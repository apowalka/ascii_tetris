#include "Grid.h"
#include "Shape.h"
#include "Utility.h"
#include <thread>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>


// This header pulls in the WebSocket++ abstracted thread support that will
// select between boost::thread and std::thread based on how the build system
// is configured.
#include <websocketpp/common/thread.hpp>

#include <nlohmann/json.hpp>

int xStart = 20;

/**
 * Define a semi-cross platform helper method that waits/sleeps for a bit.
 */
void wait_a_bit() {
#ifdef WIN32
    Sleep(100);
#else
    usleep(100000);
#endif
}

void printGrid(Grid* myGrid)
{
    GameInfo info  = myGrid->getGameInfo();
    const std::vector<std::vector<int>>& gameGrid = info.grid;
    int rows       = info.rows;
    int cols       = info.cols;
    int score      = info.score;

    cout << " Welcome to Artris " << endl;
    cout << " Score: " <<  score << endl;
    for (int i = 0; i < cols; ++i)
    {
        cout << "_";
    }
    cout << endl;
    for (int i = 0; i < rows; ++i)
    {
        cout << "|";
        for (int j = 0; j < cols; ++j)
        {
            if (gameGrid[i][j] == 1)
            {
                cout << "\u2588";
            }
            else
            {
                cout << "\u2591";
            }
        }
        cout << "|" << endl;
    }
}

void printToWindow(const GameInfo& info, WINDOW* myWin)
{
    const std::vector<std::vector<int>>& gameGrid = info.grid;
    int rows       = info.rows;
    int cols       = info.cols;
    int score      = info.score;
    int isGameOver = info.isGameOver;
    int gameId = info.gameId;

    mvwprintw(myWin, 2, 1, "%s", "Welcome to Artris");
    mvwprintw(myWin, 3, 1, "%s %d", "Score: ", score);
    mvwprintw(myWin, 3, 1, "%s %d", "GameId: ", gameId);

    int rowOffset = 5;
    int colOffset = 5;
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (gameGrid[i][j] == 1)
            {
                mvwaddch(myWin, i + rowOffset, j + colOffset, '+' | A_STANDOUT);
            }
            else if (gameGrid[i][j] == 0)
            {
                mvwaddch(myWin, i + rowOffset, j + colOffset, ' ');
            }
            else if (gameGrid[i][j] == 2)
            {
                mvwaddch(myWin, i + rowOffset, j + colOffset, '=' | A_STANDOUT);
            }
        }
    }

    wrefresh(myWin);
}

void captureInput(Grid* myGrid)
{
    while (!myGrid->isGameOver())
    {
        if (Utility::_kbhit())
        {
            char dir = getchar();
            myGrid->updateShape(dir);
        }
        usleep(10000);
    }
}

void refreshScreen(Grid* myGrid, WINDOW* win)
{
    while (!myGrid->isGameOver())
    {
        usleep(10000);
        printToWindow(myGrid->getGameInfo(), win);
    }
}

void moveDown(Grid* myGrid)
{
    int usSleep = 1000000;
    const unsigned int increment = 10;
    unsigned int nextLevelScore = increment;
    while (!myGrid->isGameOver())
    {
        myGrid->updateShape('s');
        usleep(usSleep);
        if (myGrid->getScore() >= nextLevelScore)
        {
            usSleep *= 0.6;
            nextLevelScore += increment;
        }
    }
}

class telemetry_client {
public:
    typedef websocketpp::client<websocketpp::config::asio_client> client;
    typedef websocketpp::lib::lock_guard<websocketpp::lib::mutex> scoped_lock;
    typedef client::message_ptr message_ptr;

    telemetry_client() : m_open(false),m_done(false) {
        // set up access channels to only log interesting things
        m_client.clear_access_channels(websocketpp::log::alevel::all);
        //m_client.set_access_channels(websocketpp::log::alevel::connect);
        //m_client.set_access_channels(websocketpp::log::alevel::disconnect);
        //m_client.set_access_channels(websocketpp::log::alevel::app);

        // Initialize the Asio transport policy
        m_client.init_asio();

        // Bind the handlers we are using
        using websocketpp::lib::placeholders::_1;
        using websocketpp::lib::placeholders::_2;
        using websocketpp::lib::bind;
        m_client.set_open_handler(bind(&telemetry_client::on_open,this,_1));
        m_client.set_close_handler(bind(&telemetry_client::on_close,this,_1));
        m_client.set_fail_handler(bind(&telemetry_client::on_fail,this,_1));
        m_client.set_message_handler(bind(&telemetry_client::on_message,this,_1,_2));

       int rows = 30;
       int cols = 10;
       myGrid_ = new Grid(rows, cols);

       initscr();
       cbreak();
       // this window is for the current player
       win_  = newwin(40, 20, 0, 0); // height, width, start_y, start_x
       box(win_, 0, 0); // Draw a box around the window
       wrefresh(win_);
    }

    // This method will block until the connection is complete
    void run(const std::string & uri) {
        // Create a new connection to the given URI
        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_client.get_connection(uri, ec);
        if (ec) {
            //m_client.get_alog().write(websocketpp::log::alevel::app,
            //        "Get Connection Error: "+ec.message());
            return;
        }

        // Grab a handle for this connection so we can talk to it in a thread
        // safe manor after the event loop starts.
        m_hdl = con->get_handle();

        // Queue the connection. No DNS queries or network connections will be
        // made until the io_service event loop is run.
        m_client.connect(con);

        // create game threads
        std::thread capture(captureInput, myGrid_);
        std::thread refresh(refreshScreen, myGrid_, win_);
        std::thread down(moveDown, myGrid_);

        // Create a thread to run the ASIO io_service event loop
        websocketpp::lib::thread asio_thread(&client::run, &m_client);

        // Create a thread to run the telemetry loop
        websocketpp::lib::thread telemetry_thread(&telemetry_client::telemetry_loop,this);

        telemetry_thread.join();
        capture.join();
        refresh.join();
        down.join();
        asio_thread.join();
    }

    // The open handler will signal that we are ready to start sending telemetry
    void on_open(websocketpp::connection_hdl hdl)
    {
        //m_client.get_alog().write(websocketpp::log::alevel::app,
        //    "Connection opened, starting telemetry!");

        scoped_lock guard(m_lock);
        m_open = true;
    }

    // The close handler will signal that we should stop sending telemetry
    void on_close(websocketpp::connection_hdl) {
        //m_client.get_alog().write(websocketpp::log::alevel::app,
        //    "Connection closed, stopping telemetry!");

        scoped_lock guard(m_lock);
        m_done = true;
    }

    // The fail handler will signal that we should stop sending telemetry
    void on_fail(websocketpp::connection_hdl) {
        //m_client.get_alog().write(websocketpp::log::alevel::app,
        //    "Connection failed, stopping telemetry!");

        scoped_lock guard(m_lock);
        m_done = true;
    }

    // consider creating map of connection to WINDOW
    void on_message(websocketpp::connection_hdl hdl, message_ptr msg)
    {
        const std::string& test = msg->get_payload();
        nlohmann::json doc = nlohmann::json::parse(test);

        GameInfo info;
        info.grid = doc["grid"].template get<std::vector<std::vector<int>>>();
        info.rows = doc["rows"];
        info.cols = doc["cols"];
        info.score = doc["score"];
        info.linesFilled = doc["linesFilled"];
        info.isGameOver = doc["isGameOver"];
        info.gameId = doc["gameId"];

        if (!info.isGameOver)
        {
            if (auto elem = uuidToWindow_.find(info.gameId); elem != uuidToWindow_.end())
            {
                printToWindow(info, elem->second);
                wrefresh(elem->second);
            }
            else
            {
                WINDOW* playWin = newwin(40, 20, 0, xStart); // height, width, start_y, start_x
                xStart += 20;
                box(playWin, 0, 0); // Draw a box around the window
                uuidToWindow_.insert({info.gameId, playWin});   
                printToWindow(info, playWin);
                wrefresh(playWin);
            }
        }

        unsigned int lines = doc["linesFilled"];
        if (lines > 0)
        {
            myGrid_->addPenaltyLines(lines);
        }

    }

    void telemetry_loop()
    {
        uint64_t count = 0;
        std::stringstream val;
        websocketpp::lib::error_code ec;

        while(1)
        {
            bool wait = false;
            {
                scoped_lock guard(m_lock);
                // If the connection has been closed, stop generating telemetry
                if (m_done) {break;}

                // If the connection hasn't been opened yet wait a bit and retry
                if (!m_open) {
                    wait = true;
                }
            }

            if (wait) {
                wait_a_bit();
                continue;
            }

            val.str("");
            GameInfo info = myGrid_->getGameInfo();
            const std::vector<std::vector<int>>& currGrid = info.grid;
            int rows = info.rows;
            int cols = info.cols;

            nlohmann::json doc; 
            doc["grid"] = nlohmann::json::array();
            doc["rows"] = rows;
            doc["cols"] = cols;
            doc["isGameOver"] = info.isGameOver;;
            doc["score"] = info.score;
            doc["linesFilled"] = info.linesFilled;
            doc["gameId"] = info.gameId;

            if (info.linesFilled > 0)
            {
                myGrid_->clearLinesFilled();
            }

            for (int i = 0; i < rows; ++i)
            {
                auto& eachRow = doc["grid"];
                for (int j = 0; j < cols; ++j)
                {
                    eachRow[i].push_back(currGrid[i][j]);
                }
            }

            //m_client.get_alog().write(websocketpp::log::alevel::app, val.str());
            m_client.send(m_hdl, doc.dump(), websocketpp::frame::opcode::text, ec);

            // The most likely error that we will get is that the connection is
            // not in the right state. Usually this means we tried to send a
            // message to a connection that was closed or in the process of
            // closing. While many errors here can be easily recovered from,
            // in this simple example, we'll stop the telemetry loop.
            if (ec) {
                //m_client.get_alog().write(websocketpp::log::alevel::app,
                //    "Send Error: "+ec.message());
                break;
            }

            wait_a_bit();
        }
    }
private:
    client m_client;
    websocketpp::connection_hdl m_hdl;
    websocketpp::lib::mutex m_lock;
    bool m_open;
    bool m_done;
    Grid* myGrid_;
    WINDOW* win_; //current player
    std::map<unsigned int, WINDOW*> uuidToWindow_; // other players
};

int main(int argc, char* argv[])
{
    std::string uri = "ws://localhost:9002";

    if (argc == 2) {
        uri = argv[1];
    }

    telemetry_client c;
    c.run(uri);
}
