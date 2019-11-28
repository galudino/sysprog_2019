/**
 *  @file       network.c
 *  @brief      Network source file for Asst3:
 *              The Decidedly Uncomplicated Message Broker
 *
 *  @author     Gemuele Aludino
 *  @date       27 Nov 2019
 *  @copyright  Copyright © 2019 Gemuele Aludino
 */
/**
 *  Copyright © 2019 Gemuele Aludino
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
 *  THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "network.h"

/**
 *  @brief  TODO
 * 
 *  @param[out]     cs
 *  @param[in]      inbound_handler
 *  @param[in]      outbound_handler
 * 
 *  @return TODO
 */
csocket_t *csocket_init(csocket_t *cs, 
                        void *(*inbound_handler)(void *), 
                        void *(*outbound_handler)(void *)) {
    
    return cs;
}

/**
 *  @brief  TODO
 * 
 *  @param[out]     cs
 * 
 *  @return TODO
 */
csocket_t *csocket_deinit(csocket_t *cs) {

    return cs;
}

/**
 *  @brief  TODO
 * 
 *  @param[in]      inbound_handler
 *  @param[in]      outbound_handler
 * 
 *  @return TODO
 */
void *csocket_new(void *(*inbound_handler)(void *), 
                  void *(*outbound_handler)(void *)) {
    return NULL;
}

/**
 *  @brief  TODO
 * 
 *  @param[in]      arg
 * 
 *  @return TODO
 */
void csocket_delete(void *arg) {

}

/**
 *  @brief  TODO
 * 
 *  @param[out]     cs
 *  @param[in]      hostname
 *  @param[in]      portno
 * 
 *  @return TODO
 */
int csocket_connect(csocket_t *cs, 
                    const char *hostname, 
                    int portno) {
    return 0;
}

/**
 *  @brief  TODO
 *  
 *  @param[in]  cs
 *  @param[in]  function
 *  
 *  @return TODO
 */
int csocket_start(csocket_t *cs, void *(*function)(void *)) {
    return 0;
}

/**
 *  @brief  TODO
 * 
 *  @param[out]     ss
 *  @param[in]      server_handler
 *  @param[in]      signal_handler
 * 
 *  @return TODO
 */
ssocket_t *ssocket_init(ssocket_t *ss, 
                        void *(*server_handler)(void *), 
                        void *(*signal_handler)(void *)) {
    return ss;
}

/**
 *  @brief  TODO
 * 
 *  @param[out]     ss
 * 
 *  @return TODO
 */
ssocket_t *ssocket_deinit(ssocket_t *ss) {
    return ss;
}

/**
 *  @brief  TODO
 * 
 *  @param[in]      server_handler
 *  @param[in]      signal_handler
 * 
 *  @return TODO
 */
void *ssocket_new(void *(*server_handler)(void *), 
                  void *(*signal_handler)(void *)) {
    return NULL;
}

/**
 *  @brief  TODO
 * 
 *  @param[out]     arg
 * 
 *  @return TODO
 */
void ssocket_delete(void *arg) {

}

/**
 *  @brief  TODO
 * 
 *  @param[out]     ss
 *  @param[in]      portno
 * 
 *  @return TODO
 */
int ssocket_connect(ssocket_t *ss, uint16_t portno) {
    return 0;
}
