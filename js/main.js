var myDiv = document.getElementById('counterDiv');

function updateCounterUI(counter)
{
	myDiv.innerHTML = counter; 
}

function ajaxCall() {
    var xmlhttp = new XMLHttpRequest();

    xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState == XMLHttpRequest.DONE) {
           if (xmlhttp.status == 200) {
              updateCounterUI(xmlhttp.responseText);
           }
           else {
              console.log('error', xmlhttp);
           }
        }
    };

    xmlhttp.open("GET", "GetMillis", true);
    xmlhttp.send();
}


var myDiv2 = document.getElementById('buttonsID');

function updateCounterUI2(counter)
{
	myDiv2.innerHTML = counter; 
}

function ajaxCall2() {
    var xmlhttp = new XMLHttpRequest();

    xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState == XMLHttpRequest.DONE) {
           if (xmlhttp.status == 200) {
              updateCounterUI2(xmlhttp.responseText);
           }
           else {
              console.log('error', xmlhttp);
           }
        }
    };

    xmlhttp.open("GET", "Buttons", true);
    xmlhttp.send();
}


(function scheduleAjax() {
    ajaxCall();
    setTimeout(scheduleAjax, 800);
})();

(function scheduleAjax2() {
    ajaxCall2();
    setTimeout(scheduleAjax2, 2000);
})();
