#include "modbus.hpp"

#define IP   "127.0.0.1"
#define PORT 11502

modbus_t *ctx;
modbus_mapping_t *mb_mapping;
int serv = -1;
int recv =  0;

uint8_t *query;
int header_length;

const uint16_t UT_BITS_ADDRESS = 0x130;
const uint16_t UT_BITS_NB = 0x25;

const uint16_t UT_INPUT_BITS_ADDRESS = 0x1C4;
const uint16_t UT_INPUT_BITS_NB = 0x16;
const uint8_t  UT_INPUT_BITS_TAB[] = { 0xAC, 0xDB, 0x35 };

const uint16_t UT_REGISTERS_ADDRESS = 0x160;
const uint16_t UT_REGISTERS_NB = 0x3;
const uint16_t UT_REGISTERS_NB_MAX = 0x20;

const uint16_t UT_INPUT_REGISTERS_ADDRESS = 0x108;
const uint16_t UT_INPUT_REGISTERS_NB = 0x1;
const uint16_t UT_INPUT_REGISTERS_TAB[] = { 1234 };

int main(int argc, char *argv[]) {
	cout << argv[0] << endl;

	cout << "tcp modbus ip:" << IP << " port:" << PORT << endl;

	ctx = modbus_new_tcp(IP, PORT);
	query = (uint8_t *)malloc(MODBUS_TCP_MAX_ADU_LENGTH);
	header_length = modbus_get_header_length(ctx);

	modbus_set_debug(ctx, TRUE);

    mb_mapping = modbus_mapping_new_start_address(
        UT_BITS_ADDRESS, UT_BITS_NB,
        UT_INPUT_BITS_ADDRESS, UT_INPUT_BITS_NB,
        UT_REGISTERS_ADDRESS, UT_REGISTERS_NB_MAX,
        UT_INPUT_REGISTERS_ADDRESS, UT_INPUT_REGISTERS_NB);

    if (mb_mapping == NULL) {
    	cerr << "mapping allocate error:" << modbus_strerror(errno) << endl;
    	modbus_free(ctx);
    	exit(-1);
    }

    modbus_set_bits_from_bytes(mb_mapping->tab_input_bits, 0, \
    							UT_INPUT_BITS_NB, UT_INPUT_BITS_TAB);

    for (auto i=0; i < UT_INPUT_REGISTERS_NB; i++)
    	mb_mapping->tab_input_registers[i] = UT_INPUT_REGISTERS_TAB[i];;

    serv = modbus_tcp_listen(ctx, 1); cout << "serv:" << serv << endl;
    modbus_tcp_accept(ctx, &serv);

    for (;;) {

    	do {
    		recv = modbus_receive(ctx, query);
    	} while (recv==0);

    	cout << "recv:" << recv << endl;

    	if (recv == -1 && errno != EMBBADCRC) break;

    	cout << "query[header_length]=";
    	cout << hex << (int)query[header_length] << dec << endl;

    	recv = modbus_reply(ctx, query, recv, mb_mapping);
    	if (recv == -1) break;
    }

    if (serv != -1) { close(serv); }
    modbus_mapping_free(mb_mapping);
    modbus_close(ctx);
    modbus_free(ctx);

	return 0;
}
