/*
 * Copyright (c) 2012, Peter Thorson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebSocket++ Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef WEBSOCKETPP_SERVER_ENDPOINT_HPP
#define WEBSOCKETPP_SERVER_ENDPOINT_HPP

#include <websocketpp/endpoint.hpp>

#include <iostream>

namespace websocketpp {


/// Server endpoint role based on the given config
/**
 *
 */
template <typename config>
class server : public endpoint<connection<config>,config> {
public:
    /// Type of this endpoint
	typedef server<config> type;	
	
	/// Type of the endpoint concurrency component
	typedef typename config::concurrency_type concurrency_type;
    /// Type of the endpoint transport component
	typedef typename config::transport_type transport_type;
	
	// connection policies
	//typedef role::server role_con_policy;
	//typedef concurrency_type concurrency_con_policy;
	//typedef typename transport_type::con_policy transport_con_policy;
	
	/// Type of the connections this server will create
	typedef connection<config> connection_type;
	/// Type of a shared pointer to the connections this server will create
    typedef typename connection_type::ptr connection_ptr;

    /// Type of the connection transport component
    typedef typename transport_type::transport_con_type transport_con_type;
    /// Type of a shared pointer to the connection transport component
    typedef typename transport_con_type::ptr transport_con_ptr;

	/// Type of the endpoint component of this server
	typedef endpoint<connection_type,config> endpoint_type;
	
	
	// TODO: clean up these types
	typedef typename endpoint_type::handler_type handler_type;
	typedef handler_type handler; // for backwards compatibility
	typedef typename endpoint_type::handler_ptr handler_ptr;

	explicit server(typename endpoint_type::handler_ptr default_handler) 
	  : endpoint_type(default_handler,true)
	{
		std::cout << "server constructor" << std::endl; 
	}
	
	// return an initialized connection_ptr. Call start() on this object to 
	// begin the processing loop.
	connection_ptr get_connection() {
		connection_ptr con = endpoint_type::create_connection();
				
		return con;
	}
	
	// Starts the server's async connection acceptance loop.
	void start_accept() {
		connection_ptr con = get_connection();
		
		transport_type::async_accept(
			lib::static_pointer_cast<transport_con_type>(con),
			lib::bind(
				&type::handle_accept,
				this,
				lib::placeholders::_1,
				lib::placeholders::_2
			)
		);
	}
	
	void handle_accept(connection_hdl hdl, const lib::error_code& ec) {
        connection_ptr con = endpoint_type::get_con_from_hdl(hdl);
        
        if (!con) {
            // TODO: should this be considered a server fatal error?
            std::cout << "handle_accept got an invalid handle back" << std::endl;
        } else {
            if (ec) {
                con->terminate();

                std::cout << "handle_accept: error: " << ec << std::endl;
            } else {
                con->start();
            }
        }
		
		start_accept();
	}
private:
};

} // namespace websocketpp

#endif //WEBSOCKETPP_SERVER_ENDPOINT_HPP