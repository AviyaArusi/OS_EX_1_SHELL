#ifndef CODEC_H
#define CODEC_H

/**z
 * Apply the specified codec to a message
 * 
 * @param codec The codec to use for encoding/decoding
 * @param message The message to encode/decode
 * 
 * @return The encoded/decoded message
 */
char *apply_codec(const char *codec, const char *message);

#endif
