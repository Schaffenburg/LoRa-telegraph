// Pages.h
#ifndef PAGES_H
#define PAGES_H

/*
 * Form to display strings page
 */
const char* displayForm =
"<form action='/displayString' method='post' id='myForm'>"
  "<label for='liveMode'>Live Mode:</label>"
  "<input type='checkbox' id='liveMode'><br>"
  "<textarea id='text' name='text' rows='2' cols='40'></textarea><br>"
  "<input type='submit'>"
"</form>"
"<script>"
"var liveModeCheckbox = document.getElementById('liveMode');"
"var textArea = document.getElementById('text');"
"var ws = new WebSocket('ws://' + location.hostname + ':81/');"
"textArea.addEventListener('input', function(e) {"
  "if (liveModeCheckbox.checked) {"
    "ws.send(e.target.value);"
  "}"
"});"
"</script>";

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
  "url: `/update?k=${(new URLSearchParams(window.location.search)).get(\"k\")}`,"
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
