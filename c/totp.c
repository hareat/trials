// totp.c implements a Time-based One-Time Password. see RFC 6238.
//
// see
// https://de.wikipedia.org/wiki/Time-based_One-time_Password_Algorithmus
// https://datatracker.ietf.org/doc/html/rfc4226
// https://datatracker.ietf.org/doc/html/rfc6238
// https://www.ionos.at/digitalguide/server/sicherheit/totp/
// https://www.nongnu.org/oath-toolkit/
// QR-Code generation: https://stefansundin.github.io/2fa-qr/
//                     (take care base32 password has to be used)
//
// Prerequisites for validation: sudo apt-get install oathtool
// more information: dpkg -L liboath-dev
//
// build: cc totp.c -lcrypto -o totp
// use:   ./totp 12345678901234567890
//        oathtool --base32 --time-step-size=30 --digits=6 --totp $(echo -n 12345678901234567890 | base32)
//
//        ./totp 12345678901234567890123456789012 SHA256
//        oathtool --base32 --time-step-size=30 --digits=6 --totp=SHA256 $(echo -n 12345678901234567890123456789012 | base32)
//
//        ./totp 1234567890123456789012345678901234567890123456789012345678901234 SHA512
//        oathtool --base32 --time-step-size=30 --digits=6 --totp=SHA512 $(echo -n 1234567890123456789012345678901234567890123456789012345678901234 | base32 --wrap=0)
/////////////////////////////////////////////////////////////////////

#include <openssl/evp.h>
#include <openssl/hmac.h>

#include <stdio.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 999


// Lazy hack for creating a human readable hex-string of a byte array for debug output.
// It just works for one call per printf() because the static buffer is overwritten per call.
char* bytes(const unsigned char *b, size_t len)
{
	static char buffer[BUFFER_SIZE] = "0x";	// static to use it as return value
	const size_t needed_size = 2 + len + len + 1;	// "0x" + 2 chars per byte + '\0'
	if (needed_size > BUFFER_SIZE)
		return "<Error: buffer is too small>";

	// Append the two hex chars per each byte after the starting "0x"
	const int prefix_len = 2, chars_per_byte = 2;
	for (int i = 0, j = prefix_len; i < len; ++i, j += chars_per_byte)
	{
		sprintf(buffer + j, "%02x", b[i]);
	}
	return buffer;
}


// see https://datatracker.ietf.org/doc/html/rfc6238#section-4
uint32_t totp(const char *key, const EVP_MD *evp_md, const uint64_t time_steps, const time_t start_time)
{
	//printf("DEBUG: md_size = %zu\n", evp_md->md_size);
	//printf("DEBUG: block_size = %zu\n", evp_md->block_size);
	//printf("DEBUG: ctx_size = %zu\n", evp_md->ctx_size);

	uint64_t t = time(NULL) - start_time;
	printf("still %lu seconds valid\n", time_steps - (t % time_steps));
	t = t / time_steps;
	//printf("DEBUG: t = %zu\n", t);

	// https://stackoverflow.com/questions/73392097/totp-implementation-using-c-and-openssl
	// convert t to big endian if system is little endian
	const uint32_t endianness = 0xdeadbeef;
	if ((*(const uint8_t *)&endianness) == 0xef)
	{
		t = ((t & 0x00000000ffffffff) << 32) | ((t & 0xffffffff00000000) >> 32);
		t = ((t & 0x0000ffff0000ffff) << 16) | ((t & 0xffff0000ffff0000) >> 16);
		t = ((t & 0x00ff00ff00ff00ff) <<  8) | ((t & 0xff00ff00ff00ff00) >>  8);
	};
	//printf("DEBUG: t = %zu\n", t);
	//printf("DEBUG: t (bytes) = %s\n", bytes((const unsigned char *)&t, sizeof(t)));

	const size_t keylen = strlen(key);
	//printf("DEBUG: key = %s\n", bytes((const unsigned char *)key, keylen));
	unsigned int md_len = 0;
	const uint8_t *hmac = (uint8_t*)HMAC(evp_md, key, keylen, (const unsigned char *)&t, sizeof(t), NULL, &md_len);
	//printf("DEBUG: md_len = %u\n", md_len);
	//printf("DEBUG: hmac = %s\n", bytes(hmac, md_len));

	// https://datatracker.ietf.org/doc/html/rfc4226#section-5.4
	// dynamically truncate hash
	const unsigned int offset = hmac[md_len - 1] & 0x0f;
	const uint32_t truncated =
		  (hmac[offset]   & 0x7F) << 24
		| (hmac[offset+1] & 0xFF) << 16
		| (hmac[offset+2] & 0xFF) <<  8
		| (hmac[offset+3] & 0xFF);
	// truncate to 6 digits
	return truncated % 1000000;
}


int main(const int argc, const char *argv[])
{
	if (argc < 2)
	{
		printf("usage: %s KEY [*SHA1*|SHA256|SHA512]\n", argv[0]);
		return 1;
	}
	const char *key = argv[1];
	const EVP_MD *evp_md = EVP_sha1();
	if (argc > 2)
	{
		const char *crypto = argv[2];
		if (strcmp(crypto, "SHA1") == 0)
			evp_md = EVP_sha1();
		else if (strcmp(crypto, "SHA256") == 0)
			evp_md = EVP_sha256();
		else if (strcmp(crypto, "SHA512") == 0)
			evp_md = EVP_sha512();
		else
		{
			printf("unknown hash algorithm '%s'\n", crypto);
			return 1;
		}
	}

	const int time_steps = 30, start_time = 0;
	printf("%06u\n", totp(key, evp_md, time_steps, start_time));

	return 0;
}
