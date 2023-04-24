#ifndef ENCODE_H
#define ENCODE_H

/**
 * Encode a message using the specified codec
 * 
 * @param codec The codec to use for encoding
 * @param message The message to encode
 * 
 * @return The encoded message
 */
char *encode(const char *codec, const char *message);

#endif
