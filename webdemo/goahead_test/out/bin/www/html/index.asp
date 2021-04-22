<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <link rel="stylesheet" href="../css/common.css">
    <link rel="stylesheet" href="../css/home.css">
    <script src="../My97DatePicker/WdatePicker.js"></script>
    <title>后台管理系统</title>
    
</head>
<body>
        <div class="header">
            <img src="../img/logo.png" alt="" class="logo">
            <span class="title">人脸识别系统</span>
        </div>
        <div class="main clearfix">
            <div class="menu">
                <ul class="menuList">
                    <li class="active">后台管理服务器</li>
                    <li>WLAN扫描</li>
                    <li>WLAN设置</li>
                    <li>WLAN断开</li>
                    <li>以太网设置</li>
                    <li>设备名称</li>
                    <li>设备信息</li>
                    <li>音量设置</li>
                    <li onclick="SetAntiunpack()">防拆开关</li>
                    <li>远程重启</li>
                    <li onclick="SetProtocol()">协议选择</li>
                    <li>远程开门</li>
                    <li onclick="setLanguageSelect()">语言选择</li>
                    <li>恢复出厂</li>
                    <li onclick="SetPairWaittime()">间隔设置</li>
                    <li>上传日志</li>
                    <li onclick="setLiveSelect()">识别设置</li>
                    <li>维护时间</li>
                    <li onclick="setDoorType()">开门方式</li>
                    <li>配置升级</li>

                </ul>
                <form id="toLoginOut" action="/action/LoginOut" method="post"><span class="logout" onclick='document.getElementById("toLoginOut").submit();'>退出登录</span></form>
                

            </div>
            <div class="content">
                <div class="tab-content" style="display: block;">
                    <!-- 设备IP和端口设置 -->
                    <form action="/action/Set_serverIp" method="post">
                        <ul>
                            <li><span>服务器IP:</span> <input type="text" name="ip" value="<% Get_server_ip(); %>">   </li>
                            <li><span>服务器端口:</span> <input type="text" name="port" value="<% Get_server_port(); %>">  </li>
                            <li> <input type="submit" value="确定"></li>
                        </ul>
                    </form>
            </div>
                <div class="tab-content">
	                    <!--WLAN扫描-->
	                <form action="" method="post">
	                        <ul>
	                        	<li>
                            		<% Get_Wifi_Scaned_Info_for_show(); %>
                            	</li>
							</ul>
	                </form>
                </div>
                <div class="tab-content">
                        <!-- WLAN设置 -->
                    <form action="/action/Set_wlan" method="post">
                        <ul>
                            <li><span>无线模式:</span>
                             	<select name="mod" id="modetype">
	                                <option value="DHCP">动态IP</option>
	                            </select>
	                        </li>
                            <li><span>无线名称 :</span> <input name="ssid" type="text" value="<% Get_wlan_ssid(); %>">  </li>
                            <li><span>无线密码 :</span> <input name="password" type="text" value="<% Get_wlan_password(); %>">  </li>
                            <li><span>加密方式 :</span> <input name="encrypt" type="text" value="<% Get_wlan_encrypt(); %>">  </li>
                            <li> <input type="submit" value="确定"></li>
                        </ul>
                    </form>
                </div>
                <div class="tab-content">
                        <!-- WLAN断开 -->
                    <form action="/action/Set_wlan_disconnect" method="post">
                        <ul>
                            <li> <input type="button" value="断开Wlan" onclick="showAlert('是否断开Wlan','/action/Set_wlan_disconnect')"></li>
                        </ul>
                    </form>
                </div>
                <div class="tab-content" id="Set_ethernet">
                        <!-- 以太网设置 -->
                    <form action="/action/Set_ethernet" method="post">
                        <ul>
                            <li>
                                <span>模式设置:</span>
                                <select id="ethernetmodval" name="mod">
                                    <option value="staticIP">静态IP</option>
                                    <option value="dynicIP">动态IP</option>
                                </select>
                            </li>
                            <li class="static"><span>IP:</span> <input name="ip" type="text" value="<% Get_ethernet_ip(); %>">  </li>
                            <li class="static"><span>DNS:</span> <input name="dns" type="text" value="<% Get_ethernet_dns(); %>">  </li>
                            <li class="static"><span>默认网关:</span> <input name="gateway" type="text" value="<% Get_ethernet_gateway(); %>">  </li>
                            <li class="static"><span>子网掩码:</span> <input name="mask" type="text" value="<% Get_ethernet_mask(); %>">  </li>
                            <li> <input type="submit" value="确定"></li>
                        </ul>
                    </form>
                </div>
                <div class="tab-content">
                        <!-- 设备名称 -->
                    <form action="/action/Set_deviceName" method="post">
                        <ul>
                            <li><span>名称:</span> <input type="text" value="<% Get_deviceName(); %>" name="deviceName">   </li>
                         
                            <li> <input type="submit" value="确定"></li>
                        </ul>
                    </form>
                </div>
				<div class="tab-content">
	                    <!--设备信息-->
                    <form action="" method="post">
                            <ul>
                                <li>
                                    <% Get_deviceInfo(); %>
                                </li>
                            </ul>
                    </form>
                </div>
                <div class="tab-content">
                        <!-- 音量设置 -->
                    <form action="/action/Set_volume" method="post">
                        <ul>
                            <li><span>音量设置:</span> <input type="number" value="<%Get_volume();%>" name="volume" min="0" max="100">   </li>
                            <li> <input type="submit" value="确定"></li>
                        </ul>
                    </form>
                </div>
                <div class="tab-content">
						<!--防拆开关-->
					<form action="/action/Set_antiunpack" method="post">
						<ul>
							<li>
								<span>防拆开关:</span>
                                 <select id="antiunpackSelect" name="antiunpack">
                                    <option value="1">打开</option>
                                    <option value="0">关闭</option>
                                </select>
                                <li> <input type="submit" value="确定"></li>
							</li>
						</ul>
                </div>

                <div class="tab-content">
                        <!-- 远程重启 -->
                    <form action="/action/Reboot" method="post">
                        <ul>
                            <li> <input type="button" value="重启设备" onclick="showAlert('是否重启设备','/action/Reboot')"></li>
                        </ul>
                    </form>
                </div>

                <div class="tab-content">
						<!--协议选择-->
					<form action="/action/Set_protocol_type" method="post">
						<ul>
							<li>
								<span>协议选择:</span>
                                 <select id="protocolSelect" name="protocol">
                                    <option value="0">APP PB协议</option>
                                    <option value="1">PC PB协议</option>
                                    <option value="2">http协议</option>
                                </select>
                                <li> <input type="submit" value="确定"></li>
							</li>
						</ul>
                </div>

                <div class="tab-content">
                        <!-- 远程开门 -->
                    <form action="/action/Set_opendoor" method="post">
                        <ul>
                            <li> <input name="opendoor" type="submit" name="" value="开门"></li>
                        </ul>
                    </form>
                </div>
                <div class="tab-content">
                        <!-- 语言选择 -->
                    <form action="/action/Set_language" method="post">
                        <ul>
                            <li>
                                <span>语言设置:</span>
                                 <select id="languageSelect" name="language">
                                    <option value="chinese">中文</option>
                                    <option value="english">英文</option>
                                </select>
                            </li>
                             <li> <input type="submit" value="确定"></li>
                        </ul>
                    </form>
                </div>
                <div class="tab-content">
                        <!-- 恢复出厂设置 -->
                    <form action="/action/SetDefaultConfig" method="post">
                        <ul>
                            <li> <input type="button" value="恢复出厂设置" onclick="showAlert('是否恢复出厂设置','/action/SetDefaultConfig')"></li>
                        </ul>
                    </form>
                </div>

                <div class="tab-content">
                    <!-- 间隔设置 -->
                <form action="/action/Set_pairwaittime" method="post">
                    <ul>
                        <li>
                            <span>间隔设置:</span>
                             <select  id="timeval"name="waittime">
                                <option value="1">1</option>
                                <option value="2">2</option>
                                <option value="3">3</option>
                                <option value="4">4</option>
                                <option value="5">5</option>
                                <option value="6">6</option>
                                <option value="7">7</option>
                                <option value="8">8</option>
                                <option value="9">9</option>
                                <option value="10">10</option>
                            </select>
                        </li>
                         <li> <input type="submit" value="确定"></li>
                    </ul>
                </form>
            </div>

            <div class="tab-content">
                <!--上传日志 -->
                <form action="" method="post">
                        <ul>
                            <li> <a href="<% Get_log(); %>"  target="view_window"><input type="submit" value="下载日志"></a></li>
                        </ul>
                    </form>
            </div>

            <div class="tab-content">
                    <!--识别设置 -->
                    <form action="/action/Set_livetype" method="post">
                            <ul>
                                <li>
                                    <span>识别开关:</span>
                                        <select id="liveswitch" name="liveopen">
                                        <option value="1">打开</option>
                                        <option value="0">关闭</option>
                                    </select>
                                    </li>
                                <li>
                                    <span>识别设置:</span>
                                        <select id="livetp" name="livetype">
                                        <option value="0">0</option>
                                        <option value="1">1</option>
                                        <option value="2">2</option>
                                        <option value="3">3</option>
                                        <option value="4">4</option>
                                        <option value="5">5</option>
                                        </select>
                                </li>
                                <li> <input type="submit" value="确定"></li>
                            </ul>
                        </form>
                </div>

                <div class="tab-content">
                        <!--维护时间 -->
                        <form action="/action/Set_defendtime" method="post">
                                <ul>
                                    <li>
                                        <span>开始时间：</span>  <input name="beginTime" id="d4311" class="Wdate" type="text" onclick="WdatePicker({dateFmt:'HH:mm:ss'})"/>
                                    </li>
                                    <!-- <li>
                                        <span>结束时间：</span> <input id="d4312" class="Wdate" type="text" onclick="WdatePicker({minDate:'#F{$dp.$D(\'d4311\')}',dateFmt:'HH:mm:ss'})"/>
                                      </li> -->
                                    <li> <input type="submit" value="确定"></li>
                                </ul>
                            </form>
                    </div>
                    <div class="tab-content">
                            <!--开门方式 -->
                            <form action="/action/Set_doortype" method="post">
                                    <ul>
                                        <li>
                                            <span>开门方式:</span>
                                            <select name="doortype" id="doortype">
                                                <option value="0">继电器</option>
                                                <option value="26">WG26</option>
                                                <option value="34">WG34</option>
                                                <option value="260">WG26+继电器</option>
                                                <option value="340">WG34+继电器</option>
                                            </select>
                                        </li>
                                        <li> <input type="submit" value="确定"></li>
                                    </ul>
                                </form>
                        </div>
                        <div class="tab-content">
                            <!--配置升级-->
                            <form action="/action/Set_upgrade" method="post">
                                <ul>
                                    <li>
                                    <span>配置升级:</span>
                                    <select  name="upgrade">
                                    <option value="0">手动</option>
                                    <option value="1">自动</option>
                                    </select>
                                    </li>
                                    <li> <input type="submit" value="确定"></li>
                                </ul>
                            </form>
                        </div>
            </div>




            <div class="alert" >
                <div class="alertbody">
                    <p class="alertitle">提示</p>
                    <div class="alertContent">
                        sdsdsdsds
                    </div>
                    <div class="alertFoot">
                        <form action="" method="post" style="display: inline-block;">
                             <input type="submit" value="确定" >
                        </form>
                       
                        <input type="button" value="取消" style="background: #fff;color: #333333;"  onclick="closeAlert()">
                    </div>
                </div>
            </div>
        </div>
</body>
<script src="../js/jquery-3.4.1.min.js"></script>
<script src="../js/home.js"></script>
</html>


<script type="text/javascript">
    function setLanguageSelect()//设置语言栏的当前值
    {
        var curVal = "";
        var optionVal = 0;
        <% Get_language(); %>;
        if(curVal == "chinese")
        {
            optionVal = 0;
        }
        else if(curVal == "english")
        {
            optionVal = 1;
        }
        document.getElementById('languageSelect').selectedIndex = optionVal;
    }

	function setLiveSelect()
	{
		var optionVal1 = 0;
		var optionVal2 = 0;
		optionVal1=<% Get_liveswitch(); %>;
		optionVal2=<% Get_livetype(); %>;
		if(optionVal1==1)
			optionVal1=0;
		else optionVal1=1;
		document.getElementById('liveswitch').selectedIndex = optionVal1;
		document.getElementById('livetp').selectedIndex = optionVal2;
	}
	function setDoorType()
	{
		var optionVal = 0;
		optionVal=<% Get_doortype(); %>;
		document.getElementById('doortype').selectedIndex = optionVal;
	}
	function SetPairWaittime()
	{
		var optionVal = 0;
		optionVal=<% Get_pairwaittime(); %>;
		document.getElementById('timeval').selectedIndex = optionVal;
	}
	function SetAntiunpack()
	{
		var optionVal = 0;
		var Val = 0;
		Val = <% Get_antiunpack(); %>;
		if(Val == 1)
		{
			optionVal = 0;
		}
		else if(Val == 0)
		{
			optionVal =1;
		}
		document.getElementById('antiunpackSelect').selectedIndex = optionVal;
	}
	function SetProtocol()
	{
        var curVal = "";
        var optionVal = 0;
		<% Get_protocol(); %>;
        if(curVal == "APP PB协议")
        {
            optionVal = 0;
        }
        else if(curVal == "PC PB协议")
        {
            optionVal = 1;
        }
        else if(curVal == "http 协议")
        {
            optionVal = 2;
        }
		document.getElementById('protocolSelect').selectedIndex = optionVal;
	}
	function setwlanencrypttype()
	{
		var optionVal = 0;
		optionVal=<% Get_wlan_encrypt(); %>;
		document.getElementById('encrypttype').selectedIndex = optionVal;
	}

	function SetEthernetmode()
	{
		var optionVal = 0;
		optionVal=<% Get_ethernet_mod(); %>;
		document.getElementById('ethernetmodval').selectedIndex = optionVal;
	}
</script>
