CXX=g++

BEHAVIORPATH=behaviac

INCPATH=-I. -IItem -INetWork -I$(BEHAVIORPATH) -I$(PROTOBUF_DIR)/include -I$(BOOST_ROOT) -I ThirdParty/hiredis
CXXFLAGS += $(OPT) -pipe -Wno-unused-local-typedefs -Wno-unused-but-set-variable -Wno-literal-suffix -Wall -std=c++11 -ggdb -fPIC -D_GNU_SOURCE -D_DEBUG -D__STDC_LIMIT_MACROS $(INCPATH)

LIBRARY=$(PROTOBUF_DIR)/lib/libprotobuf.a -L$(BOOST_ROOT)/stage/lib/ $(BEHAVIORPATH)/lib/libbehaviac.a ThirdParty/hiredis/libhiredis.so
LDFLAGS = -lboost_system -lboost_thread -lboost_filesystem

PROTO_SRC=P_Asset.proto P_Protocol.proto P_Server.proto
PROTO_OBJ=$(patsubst %.proto,%.pb.o,$(PROTO_SRC))
PROTO_OPTIONS=--proto_path=. --proto_path=$(PROTOBUF_DIR)/include

BASE_OBJ=WorldSession.o MessageDispatcher.o Protocol.o Entity.o Player.o Hoster.o World.o Asset.o Scene.o BehaviorTree.o Log.o
SUB_OBJ=Item/*.o 

BIN=GameServer

all: $(BIN)

clean:
	@rm -f $(BIN) $(SUB_OBJ) *.o *.pb.*

rebuild: clean all

GameServer: $(PROTO_OBJ) $(BASE_OBJ) $(SUB_OBJ) Main.o
	$(CXX) $^ -o $@ $(LIBRARY) $(LDFLAGS)

%.pb.cc: %.proto
	$(PROTOBUF_DIR)/bin/protoc $(PROTO_OPTIONS) --cpp_out=. $<

%.pb.o: %.pb.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
