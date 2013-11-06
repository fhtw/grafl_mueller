##################################
## Makefile TWMailer		##
## Alexander Grafl, if12b062	##
## Philipp Müller, if12b049	##
##################################
NAME=twmailer
VERSION=1.0
ARCHIVE=$(NAME)-$(VERSION)
FLAGS = -std=c++0x -g -Wall -Wextra -o
OPTION = -pthread -lldap -DLDAP_DEPRECATED -llber

all: $(NAME)

$(NAME):
	g++ $(FLAGS) $(NAME)_server $(NAME)_server.cpp $(OPTION); 
	g++ $(FLAGS) $(NAME)_client $(NAME)_client.cpp $(OPTION);

clean:
	rm -f $(NAME)_server; 
	rm -f $(NAME)_client;

dist:
	mkdir $(ARCHIVE); cp *.cpp $(ARCHIVE); cp *.h $(ARCHIVE);
	cp Makefile $(ARCHIVE);	tar -cvf $(ARCHIVE).tar.gz $(ARCHIVE);
	rm -rf $(ARCHIVE);

distclean:
	 rm -f $(ARCHIVE).tar.gz;
