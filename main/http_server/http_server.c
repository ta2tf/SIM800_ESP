/*
 * http_server.c
 *
 *  Created on: 14 Jun 2023
 *      Author: MertechArge014
 */



#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "esp_log.h"

#include <lwip/sockets.h>
#include <lwip/api.h>
#include <lwip/netdb.h>
#include "lwip/err.h"
#include "lwip/sys.h"



// Build http header
const static char http_html_hdr[] =
		"HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";

// Build 404 header
const static char http_404_hdr[] =
"HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n\r\n";

// Build http body
/*
const static char http_index_hml[] =
		"<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
		<title>Control</title><style>body{background-color:lightblue;font-size:24px;}</style></head>\
		<body><h1>Control</h1><a href=\"high\">ON</a><br><a href=\"low\">OFF</a></body></html>";
*/

const static char http_index_hml2[] =
		"<!DOCTYPE html>\
		<html>\
		<style>\
		.cardG{\
		     max-width: 450px;\
		     min-height: 50px;\
		     background: #0085cc;\
		     padding: 30px;\
		     box-sizing: border-box;\
		     color: #FFF;\
		     margin:20px;\
		     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);\
		}\
		.cardB{\
		     max-width: 450px;\
		     background: #02b8ff;\
		     padding: 30px;\
		     box-sizing: border-box;\
		     color: #FFF;\
		     margin:20px;\
		     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);\
		}\
		table,th {\
			  border : 1px solid black;\
			  border-collapse: collapse;\
		      padding: 5px;\
		      background: #0085cc;\
			}\
			td {\
			  border : 1px solid black;\
			  border-collapse: collapse;\
		      padding: 5px;\
		      background: #01a7ee;\
		      text-align:center;\
			}\
		    tr:nth-child(even) {\
		      background: #f2f2f2;\
		    }\
		</style>\
		\
		<body>\
			<div class=\"cardG\">\
			  <h2>CAN ID:<span id=\"IDValue\">0</span></h2>\
			</div>\
			<div class=\"cardB\">\
			<table id=\"demo\"></table>\
			</div>\
		<script>\
			setInterval(function() {\
			  getConfig();\
			}, 1500);\
			setInterval(function() {\
			  getData();\
			}, 1000);\
		    \
			function getConfig() {\
			  var xhttp = new XMLHttpRequest();\
			  xhttp.onreadystatechange = function() {\
				if (this.readyState == 4 && this.status == 200) {\
			  var xmlDoc = this.responseXML;\
		         document.getElementById(\"IDValue\").innerHTML = xmlDoc.getElementsByTagName(\"ID\")[0].childNodes[0].nodeValue;\
				}\
			  };\
			  xhttp.open(\"GET\", \"readConf\", true);\
			  xhttp.send();\
			};\
			\
		      function getData() {\
			  var xhttp = new XMLHttpRequest();\
			  xhttp.onreadystatechange = function() {\
				if (this.readyState == 4 && this.status == 200) {\
			  var i;\
			  var xmlDoc = this.responseXML;\
			  var table=\"<tr><th>CAN ID</th><th>DO</th><th>D1</th><th>D2</th><th>D3</th><th>D4</th><th>D5</th><th>D6</th><th>D7</th><th>Count</th></tr>\";\
			  var x = xmlDoc.getElementsByTagName(\"MSG\");\
			  for (i = 0; i <x.length; i++) {\
				table += \"<tr><td>\" +\
						x[i].getElementsByTagName(\"ID\")[0].childNodes[0].nodeValue +\
				\"</td><td>\" +\
						x[i].getElementsByTagName(\"D0\")[0].childNodes[0].nodeValue +\
				\"</td><td>\" +\
						x[i].getElementsByTagName(\"D1\")[0].childNodes[0].nodeValue +\
				\"</td><td>\" +\
						x[i].getElementsByTagName(\"D2\")[0].childNodes[0].nodeValue +\
				\"</td><td>\" +\
						x[i].getElementsByTagName(\"D3\")[0].childNodes[0].nodeValue +\
				\"</td><td>\" +\
						x[i].getElementsByTagName(\"D4\")[0].childNodes[0].nodeValue +\
				\"</td><td>\" +\
						x[i].getElementsByTagName(\"D5\")[0].childNodes[0].nodeValue +\
				\"</td><td>\" +\
						x[i].getElementsByTagName(\"D6\")[0].childNodes[0].nodeValue +\
				\"</td><td>\" +\
						x[i].getElementsByTagName(\"D7\")[0].childNodes[0].nodeValue +\
				\"</td><td>\" +\
						x[i].getElementsByTagName(\"CN\")[0].childNodes[0].nodeValue +\
				\"</td></tr>\";\
			  }\
			  document.getElementById(\"demo\").innerHTML = table;\
				}\
			  };\
			  xhttp.open(\"GET\", \"readID\", true);\
			  xhttp.send();\
			}\
		</script>\
		</body>\
		</html>";



//==========================================================================================
// http_server_netconn_serve
//==========================================================================================
static void http_server_netconn_serve(struct netconn *conn) {
	struct netbuf *inbuf;
	char *buf;
	u16_t buflen;
	err_t err;

	/* Read the data from the port, blocking if nothing yet there.
	 We assume the request (the part we care about) is in one netbuf */
	err = netconn_recv(conn, &inbuf);

	if (err == ERR_OK) {
		netbuf_data(inbuf, (void**) &buf, &buflen);

		/* Is this an HTTP GET command? (only check the first 5 chars, since
		 there are other formats for GET, and we're keeping it very simple )*/
		if (buflen >= 5 && strncmp("GET ",buf,4)==0) {

			/*  sample:
			 * 	GET /l HTTP/1.1
				Accept: text/html, application/xhtml+xml, image/jxr,
				Referer: http://192.168.1.222/h
				Accept-Language: en-US,en;q=0.8,zh-Hans-CN;q=0.5,zh-Hans;q=0.3
				User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.79 Safari/537.36 Edge/14.14393
				Accept-Encoding: gzip, deflate
				Host: 192.168.1.222
				Connection: Keep-Alive
			 *
			 */
			//Parse URL
			char* path = NULL;
			char* line_end = strchr(buf, '\n');
			if( line_end != NULL )
			{
				//Extract the path from HTTP GET request
				path = (char*)malloc(sizeof(char)*(line_end-buf+1));
				int path_length = line_end - buf - strlen("GET ")-strlen("HTTP/1.1")-2;
				strncpy(path, &buf[4], path_length );
				path[path_length] = '\0';
				//Get remote IP address
				ip_addr_t remote_ip;
				u16_t remote_port;
				netconn_getaddr(conn, &remote_ip, &remote_port, 0);
				printf("[ "IPSTR" ] GET %s\n", IP2STR(&(remote_ip.u_addr.ip4)),path);
			}

			/* Send the HTML header
			 * subtract 1 from the size, since we dont send the \0 in the string
			 * NETCONN_NOCOPY: our data is const static, so no need to copy it
			 */

			if(path != NULL)
			{

				if (strcmp("/readID",path)==0) {

					// displayOnWeb();
					 //netconn_write(conn, canXML, strlen(canXML), NETCONN_NOCOPY);
				//	 netconn_write(conn, canXMLlist, strlen(canXMLlist), NETCONN_NOCOPY);


				}
				else if (strcmp("/readConf",path)==0) {
					 netconn_write(conn, "<ID>100</ID>",12, NETCONN_NOCOPY);
				}
				else if (strcmp("/",path)==0)
				{
					netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1,
						NETCONN_NOCOPY);


					// Send HTML content
					netconn_write(conn, http_index_hml2, sizeof(http_index_hml2) - 1,
							NETCONN_NOCOPY);
				}
				else
				{
					netconn_write(conn, http_404_hdr, sizeof(http_404_hdr) - 1,
						NETCONN_NOCOPY);
				}

				free(path);
				path=NULL;
			}




		}

	}
	// Close the connection (server closes in HTTP)
	netconn_close(conn);

	// Delete the buffer (netconn_recv gives us ownership,
	// so we have to make sure to deallocate the buffer)
	netbuf_delete(inbuf);
}

//==========================================================================================
//==========================================================================================
// http_server
//==========================================================================================
//==========================================================================================
void http_server_task(void *pvParameters) {
	struct netconn *conn, *newconn;	//conn is listening thread, newconn is new thread for each client
	err_t err;
	conn = netconn_new(NETCONN_TCP);
	netconn_bind(conn, NULL, 80);
	netconn_listen(conn);
	do {
		err = netconn_accept(conn, &newconn);
		if (err == ERR_OK) {
			http_server_netconn_serve(newconn);
			netconn_delete(newconn);
		}

	} while (err == ERR_OK);


	netconn_close(conn);
	netconn_delete(conn);
}
