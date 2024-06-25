/* Copyright 2023 Artemis Everfree
 * https://github.com/faithanalog
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef POCSAG_H
    #define POCSAG_H

    #include <stdbool.h>
    #include <stdint.h>

    //The sync word exists at the start of every batch.
    //A batch is 16 words, so a sync word occurs every 16 data words.
    #define SYNC 0x7CD215D8

    //The idle word is used as padding before the address word, and at the end
    //of a message to indicate that the message is finished. Interestingly, the
    //idle word does not have a valid CRC code, while the sync word does.
    #define IDLE 0x7A89C197

    //One frame consists of a pair of two words
    #define FRAME_SIZE 2

    //One batch consists of 8 frames, or 16 words
    #define BATCH_SIZE 16

    //The preamble comes before a message, and is a series of alternating
    //1,0,1,0... bits, for at least 576 bits. It exists to allow the receiver
    //to synchronize with the transmitter
    #define PREAMBLE_LENGTH 576


    //These bits appear as the first bit of a word, 0 for an address word and
    //one for a data word
    #define FLAG_ADDRESS 0x000000
    #define FLAG_MESSAGE 0x100000


    //The last two bits of an address word's data represent the data type
    //0x3 for text, and 0x0 for numeric.
    #define FLAG_TEXT_DATA 0x3
    #define FLAG_NUMERIC_DATA = 0x0;

    //Each data word can contain 20 bits of text information. Each character is
    //7 bits wide, ASCII encoded. The bit order of the characters is reversed from
    //the normal bit order; the most significant bit of a word corresponds to the
    //least significant bit of a character it is encoding. The characters are split
    //across the words of a message to ensure maximal usage of all bits.
    #define TEXT_BITS_PER_WORD 20

    //As mentioned above, characters are 7 bit ASCII encoded
    #define TEXT_BITS_PER_CHAR 7

    //Length of CRC codes in bits
    #define CRC_BITS 10

    //The CRC generator polynomial
    #define CRC_GENERATOR 0b11101101001

    #define SYMRATE 38400

    /**
    * Calculate the CRC error checking code for the given word.
    * Messages use a 10 bit CRC computed from the 21 data bits.
    * This is calculated through a binary polynomial long division, returning
    * the remainder.
    * See https://en.wikipedia.org/wiki/Cyclic_redundancy_check#Computation
    * for more information.
    */
    uint32_t crc(uint32_t inputMsg);

    /**
    * Calculates the even parity bit for a message.
    * If the number of bits in the message is even, return 0, else return 1.
    */
    uint32_t parity(uint32_t x);

    /**
    * Encodes a 21-bit message by calculating and adding a CRC code and parity bit.
    */
    uint32_t encodeCodeword(uint32_t msg);

    /**
    * ASCII encode a null-terminated string as a series of codewords, written
    * to (*out). Returns the number of codewords written. Caller should ensure
    * that enough memory is allocated in (*out) to contain the message
    *
    * initial_offset indicates which word in the current batch the function is
    * beginning at, so that it can insert SYNC words at appropriate locations.
    */
    uint32_t encodeASCII(uint32_t initial_offset, char* str, uint32_t* out);

    /**
    * An address of 21 bits, but only 18 of those bits are encoded in the address
    * word itself. The remaining 3 bits are derived from which frame in the batch
    * is the address word. This calculates the number of words (not frames!)
    * which must precede the address word so that it is in the right spot. These
    * words will be filled with the idle value.
    */
    uint32_t addressOffset(uint32_t address);

    /**
    * Encode a full text POCSAG transmission addressed to (address).
    * (*message) is a null terminated C string.
    * (*out) is the destination to which the transmission will be written.
    */
    void encodeTransmission(int address, char* message, uint32_t* out);

    /**
    * Calculates the length in words of a text POCSAG message, given the address
    * and the number of characters to be transmitted.
    */
    size_t textMessageLength(int address, int numChars);

    size_t pcmTransmissionLength(
            uint32_t sampleRate,
            uint32_t baudRate,
            size_t transmissionLength);

    /**
    * sampleRate: Sample rate of output data
    * baudRate: Baud rate ouf output data
    * (*transmission): POCSAG-encoded message to transmit
    * transmissionLength: length in words of the transmission
    * (*out): Destination for output audio samples. Should be at least
    *         (transmissionLength * 32 * sampleRate / baudRate * 2) bytes in size.
    */
    void pcmEncodeTransmission(
            uint32_t sampleRate,
            uint32_t baudRate,
            uint32_t* transmission,
            size_t transmissionLength,
            uint8_t* out);

    #define SAMPLE_RATE 22050
    #define BAUD_RATE 512

    #define MIN_DELAY 1
    #define MAX_DELAY 10

#endif

