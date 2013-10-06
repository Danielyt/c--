#ifndef __stuff_h__ // if x.h hasn't been included yet...
#define __stuff_h__

#include <stdlib.h>
#include <string>
#include <iostream>
#include <cstring>
#include <algorithm>
//#include "sha1.h"

struct transition{
	int dest;
	char ch;
	transition ()
		:ch(0), dest(-1){}
};

class Transition_Hash {
	public:
		transition * array;
		int size;
		int capacity;
		char p;

		void setValues(int s, int c, int prime){
			array = (transition *) calloc (c, sizeof(transition));
			size = s;
			capacity = c;
			p = prime;
		}

		Transition_Hash(int s, int c, int prime)
			:size(s), capacity(c), p(prime), array((transition *) calloc(c, sizeof(transition))){
			//setValues(s, c, prime);
		}

		Transition_Hash()
			:size(0), capacity(2), p(13), array((transition *) calloc(2, sizeof(transition))) {			
			//setValues(0, 2, 13);
		}

		Transition_Hash(int prime) {
			setValues(0, 2, prime);
		}	

		Transition_Hash(int s, int c){
			size = s;
			capacity = c;
		}

		void insert (int c, int d) {
			
			if ((double)size / capacity > 0.9)
				resize_hash();
			
			int k = hash1(c);
			while (array[k].ch != 0)				
				k = hash2(k);
			array[k].ch = c;
			array[k].dest = d;
			size++;
		}

		int search (int c) {
			int k = hash1(c);
			int i = 0;
			while (array[k].ch != 0 && array[k].ch != c && i < size) {				
				i++;		
				k = hash2(k);				
			}

			if (array[k].ch == c)
				return k;
			return -1;
		}

		void change(char c, int d){
			int r = search(c);			
			if (r == -1)				
				insert(c, d);
			else 
				array[r].dest = d;			
		}

		~Transition_Hash(){
			free(array);
			array = 0;			
		}	

	private:
		int hash1 (int c) {
			return c % capacity;
		}

		int hash2 (int n) {
			return (n + p)%capacity;
		}

		void resize_hash() {
			Transition_Hash h1(0, 2 * capacity);			
			h1.array = (transition *) calloc (2 * capacity, sizeof(transition));		

			h1.capacity = 2*capacity;
			h1.p = p;

			for (int k = 0; k < capacity; k++) {
				if (array[k].ch != 0)
					h1.insert(array[k].ch, array[k].dest);
			}
			free(array);
			array = h1.array;
			capacity = h1.capacity;
			h1.array = 0;
		}
};

struct element{
	unsigned  message_digest[5];
	int index;
	element () :index(0){}
};

class SHA1
{
    public:

        SHA1(){
        	Reset();
        }

        virtual ~SHA1(){
    		// The destructor does nothing
		}

        /*
         *  Re-initialize the class
         */
        void Reset(){
    		Length_Low          = 0;
   			Length_High         = 0;
    		Message_Block_Index = 0;

    		H[0]        = 0x67452301;
    		H[1]        = 0xEFCDAB89;
    		H[2]        = 0x98BADCFE;
    		H[3]        = 0x10325476;
    		H[4]        = 0xC3D2E1F0;

    		Computed    = false;
    		Corrupted   = false;
		}

        /*
         *  Returns the message digest
         */
        bool Result(unsigned *message_digest_array) {
    		int i;                                  // Counter

    		if (Corrupted) {
        		return false;
    		}

    		if (!Computed)
    		{
        		PadMessage();
        		Computed = true;
    		}

    		for(i = 0; i < 5; i++)
    		{
        		message_digest_array[i] = H[i];
    		}

    		return true;
		}

        /*
         *  Provide input to SHA1
         */
        void Input(   const unsigned char *message_array,
                    unsigned            length)
		{
    		if (!length)
    		{
        		return;
    		}

		    if (Computed || Corrupted)
		    {
		        Corrupted = true;
		        return;
		    }

		    while(length-- && !Corrupted)
		    {
		        Message_Block[Message_Block_Index++] = (*message_array & 0xFF);

		        Length_Low += 8;
		        Length_Low &= 0xFFFFFFFF;               // Force it to 32 bits
		        if (Length_Low == 0)
		        {
		            Length_High++;
		            Length_High &= 0xFFFFFFFF;          // Force it to 32 bits
		            if (Length_High == 0)
		            {
		                Corrupted = true;               // Message is too long
		            }
		        }

		        if (Message_Block_Index == 64)
		        {
		            ProcessMessageBlock();
		        }

		        message_array++;
		    }
		}

        void Input(   const char  *message_array,
                    unsigned    length)
		{
		    Input((unsigned char *) message_array, length);
		}

        void Input(unsigned char message_element)
		{
		    Input(&message_element, 1);
		}

        void Input(char message_element)
		{
		    Input((unsigned char *) &message_element, 1);
		}

        SHA1& operator<<(const char *message_array)
		{
		    const char *p = message_array;

		    while(*p)
		    {
		        Input(*p);
		        p++;
		    }

		    return *this;
		}

        SHA1& operator<<(const unsigned char *message_array)
		{
		    const unsigned char *p = message_array;

		    while(*p)
		    {
		        Input(*p);
		        p++;
		    }

		    return *this;
		}

        SHA1& operator<<(const char message_element)
		{
		    Input((unsigned char *) &message_element, 1);

		    return *this;
		}

        SHA1& operator<<(const unsigned char message_element)
		{
		    Input(&message_element, 1);

		    return *this;
		}

    private:

        /*
         *  Process the next 512 bits of the message
         */
        void ProcessMessageBlock()
		{
		    const unsigned K[] =    {               // Constants defined for SHA-1
		                                0x5A827999,
		                                0x6ED9EBA1,
		                                0x8F1BBCDC,
		                                0xCA62C1D6
		                            };
		    int         t;                          // Loop counter
		    unsigned    temp;                       // Temporary word value
		    unsigned    W[80];                      // Word sequence
		    unsigned    A, B, C, D, E;              // Word buffers

		    /*
		     *  Initialize the first 16 words in the array W
		     */
		    for(t = 0; t < 16; t++)
		    {
		        W[t] = ((unsigned) Message_Block[t * 4]) << 24;
		        W[t] |= ((unsigned) Message_Block[t * 4 + 1]) << 16;
		        W[t] |= ((unsigned) Message_Block[t * 4 + 2]) << 8;
		        W[t] |= ((unsigned) Message_Block[t * 4 + 3]);
		    }

		    for(t = 16; t < 80; t++)
		    {
		       W[t] = CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
		    }

		    A = H[0];
		    B = H[1];
		    C = H[2];
		    D = H[3];
		    E = H[4];

		    for(t = 0; t < 20; t++)
		    {
		        temp = CircularShift(5,A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
		        temp &= 0xFFFFFFFF;
		        E = D;
		        D = C;
		        C = CircularShift(30,B);
		        B = A;
		        A = temp;
		    }

		    for(t = 20; t < 40; t++)
		    {
		        temp = CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1];
		        temp &= 0xFFFFFFFF;
		        E = D;
		        D = C;
		        C = CircularShift(30,B);
		        B = A;
		        A = temp;
		    }

		    for(t = 40; t < 60; t++)
		    {
		        temp = CircularShift(5,A) +
		               ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
		        temp &= 0xFFFFFFFF;
		        E = D;
		        D = C;
		        C = CircularShift(30,B);
		        B = A;
		        A = temp;
		    }

		    for(t = 60; t < 80; t++)
		    {
		        temp = CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3];
		        temp &= 0xFFFFFFFF;
		        E = D;
		        D = C;
		        C = CircularShift(30,B);
		        B = A;
		        A = temp;
		    }

		    H[0] = (H[0] + A) & 0xFFFFFFFF;
		    H[1] = (H[1] + B) & 0xFFFFFFFF;
		    H[2] = (H[2] + C) & 0xFFFFFFFF;
		    H[3] = (H[3] + D) & 0xFFFFFFFF;
		    H[4] = (H[4] + E) & 0xFFFFFFFF;

		    Message_Block_Index = 0;
		}

        /*
         *  Pads the current message block to 512 bits
         */
        void PadMessage()
		{
		    /*
		     *  Check to see if the current message block is too small to hold
		     *  the initial padding bits and length.  If so, we will pad the
		     *  block, process it, and then continue padding into a second block.
		     */
		    if (Message_Block_Index > 55)
		    {
		        Message_Block[Message_Block_Index++] = 0x80;
		        while(Message_Block_Index < 64)
		        {
		            Message_Block[Message_Block_Index++] = 0;
		        }

		        ProcessMessageBlock();

		        while(Message_Block_Index < 56)
		        {
		            Message_Block[Message_Block_Index++] = 0;
		        }
		    }
		    else
		    {
		        Message_Block[Message_Block_Index++] = 0x80;
		        while(Message_Block_Index < 56)
		        {
		            Message_Block[Message_Block_Index++] = 0;
		        }

		    }

		    /*
		     *  Store the message length as the last 8 octets
		     */
		    Message_Block[56] = (Length_High >> 24) & 0xFF;
		    Message_Block[57] = (Length_High >> 16) & 0xFF;
		    Message_Block[58] = (Length_High >> 8) & 0xFF;
		    Message_Block[59] = (Length_High) & 0xFF;
		    Message_Block[60] = (Length_Low >> 24) & 0xFF;
		    Message_Block[61] = (Length_Low >> 16) & 0xFF;
		    Message_Block[62] = (Length_Low >> 8) & 0xFF;
		    Message_Block[63] = (Length_Low) & 0xFF;

		    ProcessMessageBlock();
		}

        /*
         *  Performs a circular left shift operation
         */
        inline unsigned CircularShift(int bits, unsigned word)
		{
		    return ((word << bits) & 0xFFFFFFFF) | ((word & 0xFFFFFFFF) >> (32-bits));
		}

        unsigned H[5];                      // Message digest buffers

        unsigned Length_Low;                // Message length in bits
        unsigned Length_High;               // Message length in bits

        unsigned char Message_Block[64];    // 512-bit message blocks
        int Message_Block_Index;            // Index into message block array

        bool Computed;                      // Is the digest computed?
        bool Corrupted;                     // Is the message digest corruped?   
};

class Sha1_Hash {
	public:
		element * array;
		SHA1 sha;
		int size;
		int capacity;
		char p;

		void setValues(int s, int c, int prime){
			array = (element *) calloc (1<<c, sizeof(element));
			size = s;
			capacity = c;
			p = prime;
		}

		Sha1_Hash(int s, int c, int prime){
			setValues(s, c, prime);
		}

		Sha1_Hash(){
			setValues(0, 1, 13);
		}

		Sha1_Hash(int prime){
			setValues(0, 1, prime);
		}

		Sha1_Hash(int s, int c) {
			size = s;
			capacity = c;
		}

		void insert (unsigned new_message_digest[], int ind) {
			if ((double)size / (1<<capacity) > 0.9)
				resize_hash();

			int k = hash1(new_message_digest[4]);
			while (array[k].message_digest[0] != 0
				&& array[k].message_digest[1] != 0
				&& array[k].message_digest[2] != 0
				&& array[k].message_digest[3] != 0
				&& array[k].message_digest[4] != 0
				&& array[k].index != 0)
					k = hash2(k);
			array[k].message_digest[0] = new_message_digest[0];
			array[k].message_digest[1] = new_message_digest[1];
			array[k].message_digest[2] = new_message_digest[2];
			array[k].message_digest[3] = new_message_digest[3];
			array[k].message_digest[4] = new_message_digest[4];
			
			array[k].index = ind;
			size++;
		}

		void insert (std::string str, int ind) {
			unsigned  new_message_digest[5];

			sha.Reset();
			sha << str.c_str();

			if (!sha.Result(new_message_digest))
        		std::cout << "ERROR-- could not compute message digest\n";
    		else
				insert(new_message_digest, ind);
		}

		int search (std::string str) {
			unsigned  new_message_digest[5];

			sha.Reset();
			sha << str.c_str();

			if (!sha.Result(new_message_digest))
        		std::cout << "ERROR-- could not compute message digest\n";
        	else {
				int k = hash1(new_message_digest[4]);
				int i = 0;
				while (array[k].message_digest[0] != 0
					&& array[k].message_digest[1] != 0
					&& array[k].message_digest[2] != 0
					&& array[k].message_digest[3] != 0
					&& array[k].message_digest[4] != 0
					&& array[k].index != 0
					&& array[k].message_digest[0] != new_message_digest[0]
					&& array[k].message_digest[1] != new_message_digest[1]
					&& array[k].message_digest[2] != new_message_digest[2]
					&& array[k].message_digest[3] != new_message_digest[3]
					&& array[k].message_digest[4] != new_message_digest[4]
					&& i < size) {								
						i++;		
						k = hash2(k);				
				}

				if    (array[k].message_digest[0] == new_message_digest[0]
					&& array[k].message_digest[1] == new_message_digest[1]
					&& array[k].message_digest[2] == new_message_digest[2]
					&& array[k].message_digest[3] == new_message_digest[3]
					&& array[k].message_digest[4] == new_message_digest[4])
						return k;
				return -1;
			}
		}

		~Sha1_Hash(){
			free(array);
		}

	private:
		int hash1 (unsigned c) {
			return (c & ((1<<capacity) - 1));			
		}

		int hash2 (int n) {
			return (n + p)%(1<<capacity);
		}

		void resize_hash() {

			Sha1_Hash sh1(0, capacity + 1);
			sh1.array = (element *) calloc (1 << capacity + 1, sizeof(element));
			sh1.p = p;

			for (int k = 0; k < (1<<capacity); k++) {
				if 	  (array[k].message_digest[0] != 0
					&& array[k].message_digest[1] != 0
					&& array[k].message_digest[2] != 0
					&& array[k].message_digest[3] != 0
					&& array[k].message_digest[4] != 0
					&& array[k].index != 0)
						sh1.insert(array[k].message_digest, array[k].index);
			}
			free(array);
			array = sh1.array;
			capacity = sh1.capacity;
			sh1.array = 0;
		}
};

std::string default_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'";

class Alphabet{
	private:
		char *symbols;
		int num_of_symbols;
	public:
		Alphabet(){
			num_of_symbols = default_alphabet.length();
			symbols = new char [num_of_symbols + 1];
			std::strcpy (symbols, default_alphabet.c_str());
			std::sort(symbols, symbols + num_of_symbols);
		}
		Alphabet(std::string characters){
			num_of_symbols = characters.length();
			symbols = new char [num_of_symbols + 1];
			std::strcpy (symbols, characters.c_str());
			std::sort(symbols, symbols + num_of_symbols);
		}
		int get_index_of_symbol(char c){
			if (std::binary_search(symbols, symbols + num_of_symbols, c)) {
				char *found = std::lower_bound (symbols, symbols + num_of_symbols, c);
				return (int)(found - symbols);
			}
			return -1;
		}
		char get_symbol_at_index(int ind) {
			if (ind >= 0 && ind < num_of_symbols)
				return symbols[ind];
			return (char)0;
		}
		bool is_in_alphabet(char c){
			return std::binary_search(symbols, symbols + num_of_symbols, c);
		}
		int get_num_of_symbols(){
			return num_of_symbols;		
		}
};


#endif