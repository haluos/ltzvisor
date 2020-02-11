/*
 * Copyright (C) 2016 - 2019 Xilinx, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lwip/sockets.h"
#include "netif/xadapter.h"
#include "lwipopts.h"
#include "xil_printf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "xil_cache.h"
#include "xil_cache_l.h"

int counter = 27;

#define THREAD_STACKSIZE 1024

#define TITLE           "HTTP/1.1 200 OK \
						Content-Type: text/html\r\n\r\n\
                        <html>\n\
                        <head>\n\
                        <meta charset='utf-8'/>\n\
                        <script type='text/javascript'>\n\
						window.onload = function () {\n\
						var dps = [];\n\
						var chart = new CanvasJS.Chart('chartContainer', {\n\
							title :{\n\
								text: 'Sensor Data'\n\
							},\n\
							axisY: {\n\
								includeZero: false,\n\
								suffix: '°'\n\
							},      \n\
							axisX: {\n\
								suffix: 's'\n\
							},\n\
							data: [{\n\
								type: 'spline',\n\
								dataPoints: dps\n\
							}]\n\
						});\n\
						var xVal = 0;\n\
						var yVal = 0;\n\
						var updateInterval = 1000;\n\
						var dataLength = 20;\n\
						var updateChart = function (count) {\n\
							count = count || 1;\n\
							for (var j = 0; j < count; j++) {\n\
								var newRequest = new XMLHttpRequest();\n\
								newRequest.open('POST', '', true);\n\
								newRequest.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');\n\
								newRequest.send('SensorData=new');\n\
								newRequest.onreadystatechange = function (){\n\
									if(this.readyState == 4 && this.status === 200){\n\
										var receivedData = newRequest.response;\n\
										var dataIdx = receivedData.split('=');\n\
										// dataIdx = dataIdx[dataIdx.length - 2].split('=');\n\
										yVal = parseFloat(dataIdx[dataIdx.length - 1]);\n\
									}\n\
								};\n\
								//yVal = yVal +  Math.round(5 + Math.random() *(-5-5));\n\
								dps.push({\n\
									x: xVal,\n\
									y: yVal\n\
								});\n\
								xVal++;\n\
							}\n\
							if (dps.length > dataLength) {\n\
								dps.shift();\n\
							}\n\
							chart.render();\n\
						};\n\
						updateChart(dataLength);\n\
						setInterval(function(){updateChart()}, updateInterval);\n\
						}\n\
						</script>\n\
                        <title> Hello World </title> \n\
                        </head> \n\
                        <body>\n\
						<div id='chartContainer' style='height: 300px; width: 100%;'></div>\n\
						<script src='https://canvasjs.com/assets/script/canvasjs.min.js'></script>\n\
						</body>\n\
                        </html>"

u16_t echo_port = 80;
float last_read;
// int sensor_data;
volatile int32_t sensor_data __attribute__((section (".shared_location")));

void print_echo_app_header()
{
    xil_printf("%20s %6d %s\r\n", "echo server",
                        echo_port,
                        "$ telnet <board_ip> 7");

}

/* thread spawned for each connection */
void process_echo_request(void *p)
{
	int sd = (int)p;
	int RECV_BUF_SIZE = 2048;
	char recv_buf[RECV_BUF_SIZE];
	int n, nwrote;
	char reqResponse[] = "HTTP/1.1 200 OK Content-Type: text/xml\r\n\r\n<?xml version='1.0' encoding='UTF-8'?>";
	char data_send[strlen(reqResponse) + 100];

	while (1) {
		/* read a max of RECV_BUF_SIZE bytes from socket */
		if ((n = read(sd, recv_buf, RECV_BUF_SIZE)) < 0) {
			xil_printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
			break;
		}
		else
		{
			if(strstr(recv_buf, "GET"))
			{
				send(sd, TITLE, strlen(TITLE), 0);
				break;
			}
			if(strstr(recv_buf, "POST"))
			{
				if(strstr(recv_buf, "SensorData=new"))
				{
					memset(data_send, 0, strlen(data_send));
					sprintf(data_send, "%s", reqResponse);
					strcat(data_send, "<p>sensor_data=");
					// sprintf(&data_send[strlen(data_send)], "%d", counter);
					sprintf(&data_send[strlen(data_send)], "%f", last_read);
					strcat(data_send, "</p>");
					send(sd, data_send, strlen(data_send), 0);
					counter += (5 - rand() % 11);
					break;
				}
				break;
			}
		}

		/* break if the recved message = "quit" */
		if (!strncmp(recv_buf, "quit", 4))
			break;

		/* break if client closed connection */
		if (n <= 0)
			break;

		/* handle request */
		if ((nwrote = write(sd, recv_buf, n)) < 0) {
			xil_printf("%s: ERROR responding to client echo request. received = %d, written = %d\r\n",
					__FUNCTION__, n, nwrote);
			xil_printf("Closing socket %d\r\n", sd);
			break;
		}
	}

	/* close connection */
	close(sd);
	vTaskDelete(NULL);
}

void echo_application_thread()
{
	int sock, new_sd;
	int size;
#if LWIP_IPV6==0
	struct sockaddr_in address, remote;
	// volatile int mem_loc;
	// volatile int *mem_loc = (int *) 0x207AA8;
	// volatile uint32_t *mem_loc = (uint32_t *) 0x41200000;
	// uint32_t sensor = *mem_loc;
	// xil_printf("Memory location %x %d\n", mem_loc, sensor);
	// if(sensor)
	// {
	// 	sensor_data = sensor;
	// 	xil_printf("Sensor data is %d\n", sensor_data);
	// }
	// *mem_loc = 0;
	last_read = sensor_data = 0;
	xil_printf("NS address %x\n", &sensor_data);
	srand(time(0));

	memset(&address, 0, sizeof(address));

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return;

	address.sin_family = AF_INET;
	address.sin_port = htons(echo_port);
	address.sin_addr.s_addr = INADDR_ANY;
#else
	struct sockaddr_in6 address, remote;

	memset(&address, 0, sizeof(address));

	address.sin6_len = sizeof(address);
	address.sin6_family = AF_INET6;
	address.sin6_port = htons(echo_port);

	memset(&(address.sin6_addr), 0, sizeof(address.sin6_addr));

	if ((sock = lwip_socket(AF_INET6, SOCK_STREAM, 0)) < 0)
		return;
#endif

	if (lwip_bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
		return;

	lwip_listen(sock, 0);

	size = sizeof(remote);

	while (1) {
		// sensor_data = rand() % 300;
		Xil_DCacheFlush();
		Xil_L1DCacheFlush();
		Xil_L2CacheFlush();
		__asm("dsb");
		last_read = sensor_data;
		// last_read = (float)((float)sensor_data*100/1024)*3;
		// xil_printf("sensor data %f\n", last_read);
		// if(last_read != sensor_data)
		// {
		// 	last_read = sensor_data;
		// 	sensor_data = sensor_data - 1;
		// 	xil_printf("last read %d\n", last_read);
		// }
		// sensor = mem_loc;
		// if(sensor)
		// {
		// 	sensor_data = sensor;
		// 	*mem_loc = 0;
		// 	printf("Sensor data is %d\n", sensor_data);
		// }
		if ((new_sd = lwip_accept(sock, (struct sockaddr *)&remote, (socklen_t *)&size)) > 0) {
			sys_thread_new("echos", process_echo_request,
				(void*)new_sd,
				THREAD_STACKSIZE*2,
				DEFAULT_THREAD_PRIO);
		}
		vTaskDelay(5/portTICK_RATE_MS);
	}
}
