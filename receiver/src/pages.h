// Pages.h
#ifndef PAGES_H
#define PAGES_H

/*
 * Form to display strings page
 */
const char* displayForm =
"<form id='StringForm' method='post'>"
  "<textarea name='text' rows='2' cols='40'></textarea><br>"
  "<input type='submit' value='Senden'>&nbsp;"
  "<label for='liveMode'>Live Mode:</label>"
  "<input type='checkbox' id='liveMode'><br>"
"</form>"
"<form id='RawForm' method='post'>"
  "<input type='number' name='byte' step='1' pattern='\\d+' max='255'></input><br>"
  "<input type='submit' value='Senden'>"
"</form>"
"<form id='LedForm' method='post'>"
  "<input type='hidden' id='LedValue' name='led'>"
  "<label for='LedValue'>Enable LEDs:</label>"
  "<input type='submit' value='Off' onclick='document.getElementById('LedValue').value='Off''>"
  "<input type='submit' value='On' onclick='document.getElementById('LedValue').value='On''>"
"</form>"
"<div id='message'>Status</div>"
"<br><a href='/ticker'>Ticker</a>"
"<script>"
"function handleSubmit(formId) {"
  "document.getElementById(formId).addEventListener('submit', function(event) {"
    "event.preventDefault();"
    "var xhr = new XMLHttpRequest();"
    "xhr.open('POST', '/'+formId, true);"
    "xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');"
    "var data = new FormData(event.target);"
    "xhr.send([...data.entries()].map(e => encodeURIComponent(e[0])+'='+encodeURIComponent(e[1])).join('&'));"
    "xhr.onloadend = function() {"
      "document.getElementById('message').textContent = 'Response (Status ' + xhr.status + '): ' + xhr.responseText;"
    "}"
  "});"
"}"

"['StringForm', 'RawForm', 'LedForm'].forEach(handleSubmit);"
"</script>"
"<script>"
"var liveModeCheckbox = document.getElementById('liveMode');"
"var textArea = document.getElementsByName('text')[0];"
"var ws = new WebSocket('ws://' + location.hostname + ':81/');"
"textArea.addEventListener('input', function(e) {"
  "if (liveModeCheckbox.checked) ws.send(e.target.value);"
"});"
"</script>";

const char* tickerForm =
"<form id='tickerForm' action='/tickerForm' method='post'>"
  "<label for='Line1'>Line 1:</label>"
  "<input type='text' id='Line1' name='Line1'><br>"
  "<label for='Line2'>Line 2:</label>"
  "<input type='text' id='Line2' name='Line2'><br>"
  "<label for='Line3'>Line 3:</label>"
  "<input type='text' id='Line3' name='Line3'><br>"
  "<label for='timeout_ms'>Timeout (ms):</label>"
  "<input type='number' name='timeout_ms' step='250' default='2000'></input><br>"
  "<label for='toggle'>Enable:</label>"
  "<input type='checkbox' id='toggle' name='toggle'><br>"
  "<input type='submit' value='Senden'>"
"</form>"
"<script>"
"handleSubmit('tickerForm');"
"</script>";

/*
 * Login page
 */
const char* colorPickerIndex =
"<head>"
"<script src='https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.5.1/jscolor.min.js' integrity='sha512-/e+XGe8oSD9M1t0NKNCrUlRsiyeFTiZw4+pmf0g8wTbc8IfiLwJsjTODc/pq3hKhKAdsehJs7STPvX7SkFSpOQ==' crossorigin='anonymous' referrerpolicy='no-referrer'></script>"
"</head><body><p>Color: <input data-jscolor=\"{onInput:\'update(this)\'}\"></p>"
"<script>"
  "var ws = new WebSocket('ws://' + location.hostname + ':81/');"
  "ws.onopen = function () {"
    "console.log('Connected via WebSocket');"
  "};"
  "ws.onerror = function (error) {"
    "console.log('WebSocket Error: ', error);"
  "};"
  "function update(picker) {"
    "console.log('{RGB}='+picker.toHEXString());"
    "ws.send('{RGB}='+picker.toHEXString());"
  "}"
"</script></body></html>";

/*
 * Login page
 */
const char* loginIndex =
 "<form action='/serverIndex' method='get'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 OTA Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<td>Username:</td>"
        "<td><input size=25><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Key:</td>"
            "<td><input type='Password' size=25 name='k'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>";

/*
 * Server Index Page
 */

const char* serverIndex =
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: `/update?k=${(new URLSearchParams(window.location.search)).get('k')}`,"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')"
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";

/*
 * CSV Upload page
 */
const char* csvUploadPage =
"<form method='POST' action='/uploadCSV' enctype='multipart/form-data' id='upload_csv_form'>"
   "<label for='csvFile'>Upload CSV File:</label>"
   "<input type='file' name='csvFile' accept='.csv'><br>"
   "<input type='submit' value='Upload'>"
"</form>"
"<script>"
  "$('#upload_csv_form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_csv_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/uploadCSV',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "success:function(d, s) {"
  "console.log('File uploaded successfully!')"
 "},"
 "error: function (a, b, c) {"
 "console.log('Error during file upload.');"
 "}"
 "});"
 "});"
 "</script>";

#endif
