//
// Created by paolo on 08/01/20.
//

#ifndef VASO2_DATASERVICE_H
#define VASO2_DATASERVICE_H


#include <esp_err.h>
#include <http_server.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t httpData(httpd_req_t *req);

#ifdef __cplusplus
}
#endif
#endif //VASO2_DATASERVICE_H
