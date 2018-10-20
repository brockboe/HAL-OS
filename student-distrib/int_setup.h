void int_setup();
void C_int_dispatcher(unsigned long interrupt);
void install_handler(int vector_number, void handler());
