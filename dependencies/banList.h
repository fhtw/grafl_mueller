#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <list>
#include <iterator>
#include <ctime>


using namespace std;



class bannedIP
{
    public:
        bannedIP(string IP)
        {
            _IP = IP;
            time(&_now);
            _tries = 0;
        }
        virtual ~bannedIP()
        {
            _IP = "";
        }
        int getTries()
        {
        	return _tries;
        }
        int incrTries()
        {
        	_tries++;
        }
        string getIP()
        {
            return _IP;
        }
        double getTimeDiff()
        {
            return difftime(time(NULL),_now);
        }

    private:
        string _IP;
        time_t _now;
        int _tries;
};

class banHandler
{
    public:
        banHandler(int BANTIME)
        {
        	_BANTIME = BANTIME;
        }
        virtual ~banHandler()
        {
            _banList.clear();
        }
        void addIP(string IP)
        {
            bannedIP *bip = new bannedIP(IP);
            _banList.push_back(bip);
            return;
        }
        double checkIP(string IP)
        {
            this->checkTime();

            for(iter = _banList.begin(); iter != _banList.end() ; iter++)
            {
                if ((*iter)->getIP() == IP) 
                {
            		if((*iter)->getTries() == 3)
                	{
                		return (((double)_BANTIME) - (*iter)->getTimeDiff());
            		}
            		else
            		{
            			(*iter)->incrTries();
            		}
            	}
            }
            return 0;
        }

        void checkTime()
        {
            list <bannedIP*> deleteList;
            for(iter = _banList.begin(); iter != _banList.end() ; iter++)
            {
                if ((*iter)->getTimeDiff() >= (double) _BANTIME)
                {
                    deleteList.push_back(*iter);
                }
            }
            for(iter = deleteList.begin(); iter != deleteList.end() ; iter++)
            {
                cout << "Delete: " << ((*iter)->getIP()) << endl;
                _banList.remove(*iter);
            }
            deleteList.clear();
            return;
        }
    private:
        list < bannedIP* > _banList;
        list < bannedIP* >::iterator iter;
        int _BANTIME;
};




