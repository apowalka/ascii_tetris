#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <iostream>
#include <ncurses.h>
#include <set>


typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;
typedef std::set<websocketpp::connection_hdl,std::owner_less<websocketpp::connection_hdl>> con_list;

//WINDOW* win;
con_list m_connections;

void on_open(server* s, websocketpp::connection_hdl hdl) {
    m_connections.insert(hdl);
}

// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg)
{
    if (msg->get_payload() == "stop-listening") {
        s->stop_listening();
        return;
    }

    // take data received from player1 and send it to player2
    server::connection_ptr currCon = s->get_con_from_hdl(hdl);
    for (auto con : m_connections)
    {
        server::connection_ptr someCon= s->get_con_from_hdl(con);
        // echo the data back to all but this connection
        if (currCon != someCon)
        {
            try {
                s->send(con, msg->get_payload(), msg->get_opcode());
            } catch (websocketpp::exception const & e) {
                std::cout << "Echo failed because: "
                          << "(" << e.what() << ")" << std::endl;
            }
        }
    }
}

int main() {
    // Create a server endpoint
    server echo_server;

    try {
        // Set logging settings
        echo_server.set_access_channels(websocketpp::log::alevel::none);
        //echo_server.set_access_channels(websocketpp::log::alevel::connect);
        //echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        echo_server.init_asio();

        // Register our message handler
        echo_server.set_message_handler(bind(&on_message,&echo_server,::_1,::_2));
        echo_server.set_open_handler(bind(&on_open,&echo_server,::_1));

        // Listen on port 9002
        echo_server.set_reuse_addr(true);
        echo_server.listen(9002);

        // Start the server accept loop
        echo_server.start_accept();

        // Start the ASIO io_service run loop
        echo_server.run();
    } catch (websocketpp::exception const & e) {
        //std::cout << e.what() << std::endl;
    } catch (...) {
        //std::cout << "other exception" << std::endl;
    }
}
