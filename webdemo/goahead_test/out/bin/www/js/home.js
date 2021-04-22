window.onload= function(){
   $('.menuList li').click(function(){
       $(this).addClass("active");
       $(this).siblings('li').removeClass('active');
       $(".content .tab-content").hide();
       $(".content .tab-content").eq($(this).index()).show();
   })




}
function showAlert(content,url){
    $(".alert").find(".alertContent").text(content);
    $(".alert").find("form").attr('action',url);
    $(".alert").show();
}
 function closeAlert(){
    $(".alert").hide();
 }

 function  Set_ethernet(){
    var e = window.event || arguments.callee.caller.arguments[0];
     var value  = $(e.target).val();
     if(value=='dynicIP'){
         $('#Set_ethernet').find('.static').hide();
     }else{
        $('#Set_ethernet').find('.static').show();
     }
 }
