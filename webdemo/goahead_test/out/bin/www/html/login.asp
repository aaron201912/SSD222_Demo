<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <link rel="stylesheet" href="../css/common.css">
    <title>后台管理系统</title>
</head>
<body>
    <img src="../img/logo.png" alt="" class="logo">
    <div class="h1">人脸识别系统</div>
    <form action="/action/Login" method="post">
        <ul class="from-class">
            <li><input type="text" name="user" placeholder="用户名"></li>
            <li><input type="password"  name="pass" placeholder="密码"></li>
            <li><input type="submit" value="登录"></li>
        </ul>
    </form>
</body>
</html>
<style>
.from-class{
    width:400px;
    margin:30px auto;
    color:#fff;
    
}
.from-class li{
    height: 50px;
    width: 100%;
    text-align: center;
}
.from-class li input{
    height: 30px;
    line-height: 30px;
    background: rgba(255,255,255,0.1);
    border: none;
    padding-left: 10px;
    border-radius: 3px;
    width: 246px;
    color:#fff;
}
.from-class li [type='submit']{
    width: 100px;
    padding-left: 0px;
    background: #2a68c9;
    color: #fff;
}
.h1{
    text-align: center;
    font-size: 30px;
    color:#fff;
    font-weight: bold;
    margin:10px auto;
}
.logo{
    width: 100px;
    display: block;
    margin: 100px auto 0 auto;
}
</style>