#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <wait.h>
#include <sys/prctl.h>
#include "sstardisp.h"
#include "entry/EasyUIContext.h"


#define MAKE_YUYV_VALUE(y,u,v)  ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK              MAKE_YUYV_VALUE(0,128,128)

#define FD_MAX	1024
#define SSD_IPC "/tmp/ssd_apm_input"
#define SVC_IPC "/tmp/brown_svc_input"
#define UI_IPC	"/tmp/zkgui_msg_input"

typedef enum
{
  IPC_KEYEVENT = 0,
  IPC_COMMAND,
  IPC_LOGCMD,
  IPC_EVENT_MAX,
} IPC_EVENT_TYPE;

typedef enum
{
  IPC_COMMAND_EXIT = 0,
  IPC_COMMAND_SUSPEND,
  IPC_COMMAND_RESUME,
  IPC_COMMAND_RELOAD,
  IPC_COMMAND_BROWN_GETFOCUS,
  IPC_COMMAND_BROWN_LOSEFOCUS,
  IPC_COMMAND_APP_START_DONE,
  IPC_COMMAND_APP_STOP_DONE,
  IPC_COMMAND_SETUP_WATERMARK,
  IPC_COMMAND_APP_START,
  IPC_COMMAND_APP_STOP,
  IPC_COMMAND_UI_EXIT,
  IPC_COMMAND_APP_SUSPEND,
  IPC_COMMAND_APP_SUSPEND_DONE,
  IPC_COMMAND_APP_RESUME,
  IPC_COMMAND_APP_RESUME_DONE,
  IPC_COMMAND_MAX,
} IPC_COMMAND_TYPE;

typedef struct {
  uint32_t EventType;
  uint32_t Data;
  char StrData[256];
} IPCEvent;

char gsd20xipaddr[32] = {1};

static int running = 0;
static int delay = 1;
static int counter = 0;
static char *conf_file_name = NULL;
static char *pid_file_name = NULL;
static int pid_fd = -1;
static char *app_name = NULL;
static FILE *log_stream;
static int child_pid = 0;

class IPCNameFifo {
public:
  IPCNameFifo(const char* file): m_file(file) {
    unlink(m_file.c_str());
    printf("mkfifo: %s\n",m_file.c_str());
    m_valid = !mkfifo(m_file.c_str(), 0777);
  }

  ~IPCNameFifo() {
    unlink(m_file.c_str());
  }

  inline const std::string& Path() { return m_file; }
  inline bool IsValid() { return m_valid; }

private:
  bool m_valid;
  std::string m_file;
};

class IPCInput {
public:
  IPCInput(const std::string& file):m_fd(-1),m_file(file),m_fifo(file.c_str()){
  printf("construct ipcinput\n");}

  virtual ~IPCInput() {
    Term();
  }
  bool Init() {
    if (!m_fifo.IsValid()){
        printf("%s non-existent!!!! \n",m_fifo.Path().c_str());
        return false;
    }
    if (m_fd < 0) {
      m_fd = open(m_file.c_str(), O_RDWR | O_NONBLOCK);
    }
    return m_fd >= 0;
  }

  int Read(IPCEvent& evt) {
    if (m_fd >= 0) {
      return read(m_fd, &evt, sizeof(IPCEvent));
    }
    return 0;
  }

  void Term() {
    if (m_fd >= 0) {
      close(m_fd);
      m_fd = -1;
    }
  }

private:
  int m_fd;
  std::string m_file;
  IPCNameFifo m_fifo;
};

class IPCOutput {
    public:
	  IPCOutput(const std::string& file):m_fd(-1), m_file(file) {
	}

	virtual ~IPCOutput() {
		Term();
	}

	bool Init() {
		if (m_fd < 0) {
			m_fd = open(m_file.c_str(), O_WRONLY | O_NONBLOCK);
		}
		return m_fd >= 0;
	}

	void Term() {
		if (m_fd >= 0) {
			close(m_fd);
			m_fd = -1;
		}
	}

	void Send(const IPCEvent& evt) {
		if (m_fd >= 0) {
			write(m_fd, &evt, sizeof(IPCEvent));
		}
	}

private:
	int m_fd;
	std::string m_file;
};
/**
 * \brief Read configuration from config file
 */
int read_conf_file(int reload)
{
    FILE *conf_file = NULL;
    int ret = -1;

    if (conf_file_name == NULL) return 0;

    conf_file = fopen(conf_file_name, "r");

    if (conf_file == NULL) {
        syslog(LOG_ERR, "Can not open config file: %s, error: %s",
                conf_file_name, strerror(errno));
        return -1;
    }

    ret = fscanf(conf_file, "%d", &delay);

    if (ret > 0) {
        if (reload == 1) {
            syslog(LOG_INFO, "Reloaded configuration file %s of %s",
                conf_file_name,
                app_name);
        } else {
            syslog(LOG_INFO, "Configuration of %s read from file %s",
                app_name,
                conf_file_name);
        }
    }

    fclose(conf_file);

    return ret;
}

/**
 * \brief This function tries to test config file
 */
int test_conf_file(char *_conf_file_name)
{
    FILE *conf_file = NULL;
    int ret = -1;

    conf_file = fopen(_conf_file_name, "r");

    if (conf_file == NULL) {
        fprintf(stderr, "Can't read config file %s\n",
            _conf_file_name);
        return EXIT_FAILURE;
    }

    ret = fscanf(conf_file, "%d", &delay);

    if (ret <= 0) {
        fprintf(stderr, "Wrong config file %s\n",
            _conf_file_name);
    }

    fclose(conf_file);

    if (ret > 0)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}

void server_on_exit()
{
	printf("child process exit...\n");
    EASYUICONTEXT->deinitEasyUI();
}

/**
 * \brief Callback function for handling signals.
 * \param   sig identifier of signal
 */
void handle_signal(int sig)
{
    if (sig == SIGINT) {
        fprintf(log_stream, "Debug: stopping daemon ...\n");
        /* Unlock and close lockfile */
        if (pid_fd != -1) {
            lockf(pid_fd, F_ULOCK, 0);
            close(pid_fd);
        }
        /* Try to delete lockfile */
        if (pid_file_name != NULL) {
            unlink(pid_file_name);
        }
        running = 0;
        /* Reset signal handling to default behavior */
        signal(SIGINT, SIG_DFL);
    } else if (sig == SIGHUP) {
        fprintf(log_stream, "Debug: reloading daemon config file ...\n");
        read_conf_file(1);
    } else if (sig == SIGCHLD) {
        fprintf(log_stream, "Debug: received SIGCHLD signal\n");
    }
}

void handler(int signo)
{
    pid_t id;
    if(child_pid > 0)
    {
        while((id=waitpid(child_pid,NULL,WNOHANG))>0)
        {
			printf("handle signal....childPid=%d\n", child_pid);
            syslog(LOG_INFO, "wait child success:%d", id);
        }
        syslog(LOG_INFO, "child quit!%d", getpid());
        child_pid = 0;
    }
}



/**
 * \brief This function will daemonize this app
 */
static void daemonize()
{
    pid_t pid = 0;
    int fd;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* On success: The child process becomes session leader */
    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    /* Ignore signal sent from child to parent process */
    //signal(SIGCHLD, SIG_IGN);
    signal(SIGCHLD,handler);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");

    /* Close all open file descriptors */
    for (fd = sysconf(_SC_OPEN_MAX); fd > 0; fd--) {
        close(fd);
    }

    /* Reopen stdin (fd = 0), stdout (fd = 1), stderr (fd = 2) */
    //stdin = fopen("/dev/null", "r");
    //stdout = fopen("/dev/null", "w+");
    //stderr = fopen("/dev/null", "w+");

    /* Try to write PID of daemon to lockfile */
    if (pid_file_name != NULL)
    {
        char str[256];
        pid_fd = open(pid_file_name, O_RDWR|O_CREAT, 0640);
        if (pid_fd < 0) {
            /* Can't open lockfile */
            exit(EXIT_FAILURE);
        }
        if (lockf(pid_fd, F_TLOCK, 0) < 0) {
            /* Can't lock file */
            exit(EXIT_FAILURE);
        }
        /* Get current PID */
        sprintf(str, "%d\n", getpid());
        /* Write PID to lockfile */
        write(pid_fd, str, strlen(str));
    }
}

void clear_fd()
{
	for (int i = 3; i < FD_MAX - 1; i++)
	{
		close(i);
	}
}

/**
 * \brief Print help for this application
 */
void print_help(void)
{
    printf("\n Usage: %s [OPTIONS]\n\n", app_name);
    printf("  Options:\n");
    printf("   -h --help                 Print this help\n");
    printf("   -c --conf_file filename   Read configuration from the file\n");
    printf("   -t --test_conf filename   Test configuration file\n");
    printf("   -l --log_file  filename   Write logs to the file\n");
    printf("   -d --daemon               Daemonize this application\n");
    printf("   -p --pid_file  filename   PID file used by daemonized app\n");
    printf("\n");
}
int createEasyui(void)
{
    int pid;

    pid = fork();

    /* An error occurred */
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if (pid > 0) {
        //exit(EXIT_SUCCESS);
    }
    if(!pid)
    {
    #if 0
        /* Fork off for the second time*/
        pid = fork();

        /* An error occurred */
        if (pid < 0) {
            exit(EXIT_FAILURE);
        }

        /* Success: Let the parent terminate */
        if (pid > 0) {
            exit(EXIT_SUCCESS);
        }
    #endif
		clear_fd();

		atexit(server_on_exit);

        prctl(PR_SET_NAME, "zkgui_ui", NULL, NULL, NULL);
        if (EASYUICONTEXT->initEasyUI())
        {
            EASYUICONTEXT->runEasyUI();
            EASYUICONTEXT->deinitEasyUI();
        }
    }

    return pid;
}



/* Main function */
int main(int argc, const char *argv[])
{
	int forkPid = -1;
    app_name = (char *)argv[0];

    //daemonize();
    /* Open system log and write message to it */
    //openlog(argv[0], LOG_PID|LOG_CONS, LOG_DAEMON);
    //syslog(LOG_INFO, "Started %s", app_name);

    /* This global variable can be changed in function handling signal */
    running = 1;
    forkPid = createEasyui();
	signal(SIGCHLD, SIG_IGN);		// 子进程信息状态被丢弃，自动回收资源

	if (forkPid > 0)
	{
		sstar_disp_init();

		child_pid = forkPid;
		printf("create ui process, id is %d\n", child_pid);

		IPCEvent getevt;

		IPCInput ssdinput(SSD_IPC);
		if(!ssdinput.Init())
		{
			printf("ipc init fail\n");
			return 0;
		}

		//syslog(LOG_INFO, "ssdinput end");
		/* Never ending loop of server */
		while (running == 1)
		{

			/* TODO: dome something useful here */
			memset(&getevt,0,sizeof(IPCEvent));
			if(ssdinput.Read(getevt) > 0)
			{
				printf("main process get msg %d\n", getevt.Data);
				//syslog(LOG_ERR,"Get EventEventType[%d] Data[%d] StrData[%s]",getevt.EventType,getevt.Data,getevt.StrData);
				if(getevt.EventType == IPC_COMMAND && getevt.Data == IPC_COMMAND_APP_START_DONE)
				{
					//syslog(LOG_ERR,"Browser Star done!!!!");
				}

				if(getevt.EventType == IPC_COMMAND && getevt.Data == IPC_COMMAND_APP_STOP_DONE)
				{
					//syslog(LOG_ERR,"Browser Stop done!!!!");
					forkPid = createEasyui();
					if (forkPid > 0)
					{
						child_pid = forkPid;
						printf("create ui process, id is %d\n", child_pid);
					}
				}

				if (getevt.EventType == IPC_COMMAND && getevt.Data == IPC_COMMAND_UI_EXIT)
				{
					printf("recv ui exit msg %d\n", IPC_COMMAND_UI_EXIT);

					if (child_pid > 0)
					{
						IPCOutput sendToBrowser(SVC_IPC);
						if(!sendToBrowser.Init())
						{
							printf("Brown process Not start!!!\n");
							sendToBrowser.Term();
						}
						IPCEvent sendevt;
						memset(&sendevt,0,sizeof(IPCEvent));
						sendevt.EventType = IPC_COMMAND;
						sendevt.Data = IPC_COMMAND_APP_START;  //IPC_COMMAND_APP_STOP to stop browser fg
						sendToBrowser.Send(sendevt);
					}
				}

				if (getevt.EventType == IPC_COMMAND && getevt.Data == IPC_COMMAND_APP_SUSPEND)
				{
					printf("recv ui suspend msg %d\n", IPC_COMMAND_APP_SUSPEND);

					if (child_pid > 0)
					{
						sstar_disp_Deinit();

						IPCOutput sendToUI(UI_IPC);
						if(!sendToUI.Init())
						{
							printf("UI process Not start!!!\n");
							sendToUI.Term();
						}
						IPCEvent sendevt;
						memset(&sendevt,0,sizeof(IPCEvent));
						sendevt.EventType = IPC_COMMAND;
						sendevt.Data = IPC_COMMAND_APP_SUSPEND_DONE;
						sendToUI.Send(sendevt);
					}
				}

				if (getevt.EventType == IPC_COMMAND && getevt.Data == IPC_COMMAND_APP_RESUME)
				{
					printf("recv ui resume msg %d\n", IPC_COMMAND_APP_RESUME);

					if (child_pid > 0)
					{
						sstar_disp_init();

						IPCOutput sendToUI(UI_IPC);
						if(!sendToUI.Init())
						{
							printf("UI process Not start!!!\n");
							sendToUI.Term();
						}
						IPCEvent sendevt;
						memset(&sendevt,0,sizeof(IPCEvent));
						sendevt.EventType = IPC_COMMAND;
						sendevt.Data = IPC_COMMAND_APP_RESUME_DONE;
						sendToUI.Send(sendevt);
					}
				}
			}


			/* Real server should use select() or poll() for waiting at
			 * asynchronous event. Note: sleep() is interrupted, when
			 * signal is received. */
			sleep(delay);
		}
	}

    /* Write system log and close it. */
    //syslog(LOG_INFO, "Stopped %s", app_name);
    //closelog();

    return 0;
}
