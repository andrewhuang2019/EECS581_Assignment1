# AI usage and disclosure requirements
You are required to use a generative AI tool for all or at least part of this assignment's code generation, and to document that usage professionally. The purpose is not to test whether you can write this parser unassisted — it is to test whether you can direct an AI tool effectively, evaluate its output critically, and communicate that process clearly.

## General disclosure

### Clearly state which GAI tool was used (e.g., ChatGPT, Copilot, Gemini, etc.), and specify the version if known (e.g., GPT-4, Claude 3.5).
Claude Sonnet 5 (Medium) is the generative AI used for this assignment.

### Indicate the date(s) the tool was consulted.
First prompt: September 3, 2026
Second prompt: September 4, 2026
Third Prompt: September 4, 2026

## Code attribution

### Copy the exact prompt(s) you used to generate the code.
First Prompt (September 3, 2026):
Generate me c++ code which takes in an input string with these requirements:

// Returns true if a valid address was found, false otherwise.
// On success: outAddress holds the 32-bit value,
// and outPort holds the port number, or -1 if no port was present.
// On failure: outAddress is set to 0 and outPort is set to -1.
bool extractIPv4(const std::string& str, unsigned long& outAddress, int& outPort);

Every character which is not a digit, period, or colon should be skipped. An address is 4 octets, with each octet 1–3 digits, value 0–255. An optional port may be added, which may follow the last octet: 1–5 digits, value 0–65535. If a colon is present, the port must be fully valid or the entire match — address included — is rejected.

This code may not use any string to number conversion function, any address parsing library function, or any regular expression facility. 

Other requirements include:

only one address may be extracted per input line
reject anything that does not exactly fit the grammar above
On success, print: Extracted IPv4 address: A.B.C.D (decimal value: N, port: P) where N is the 32-bit decimal value and P is the port number or the literal text none.
sample outputs:

Enter a string (or 'END' to quit): connecting to 192.168.1.1 now
Extracted IPv4 address: 192.168.1.1 (decimal value: 3232235777, port: none)
Enter a string (or 'END' to quit): server=10.0.0.255:8080end
Extracted IPv4 address: 10.0.0.255 (decimal value: 167772415, port: 8080)
Enter a string (or 'END' to quit): 192a168.1.1.1
Extracted IPv4 address: 168.1.1.1 (decimal value: 2818638081, port: none)
Enter a string (or 'END' to quit): 192.168.1.1.
Invalid input: no valid IPv4 address found
Enter a string (or 'END' to quit): Connection from 192.168.1.1 refused
Extracted IPv4 address: 192.168.1.1 (decimal value: 3232235777, port: none)
Enter a string (or 'END' to quit): 192.168.01.1
Invalid input: no valid IPv4 address found
Enter a string (or 'END' to quit): 1.2.3.4:99999
Invalid input: no valid IPv4 address found
Enter a string (or 'END' to quit): 12.34.56
Invalid input: no valid IPv4 address found
Enter a string (or 'END' to quit): no number here
Invalid input: no valid IPv4 address found
Enter a string (or 'END' to quit): END
Program terminated.

Second Prompt (September 4, 2026): 
does this cover this case: 192abc.1231.awoefja.129.30.3.4asda.0.1

Third Prompt (September 4, 2026):
Further specification says that it must reject everything that does not exactly match the grammar from before, which includes: wrong octet count, empty octet, out-of-range octet or port, a disallowed leading zero, a second colon, a colon not immediately after the fourth octet, or a stray period/colon directly adjacent to an otherwise-valid address.

### Document which parts of the code were AI-generated versus student-written.
All comments and functions which are not specified to be student-written were AI-generated

### Note any modifications you made to the AI-generated output, and why.
No modifications were made to the AI generated output, because I had ran it against many test cases and found that it covered them.

## Verification statement
### Confirm that you understand every line of the submitted code — no blind copying.
I confirm that I have read every line of the submitted code. The GAI model generated 7 functions, each of which were created to consolodate a piece of code.

## State that the code has been tested and works as intended.


### Acknowledge any known bugs, limitations, or unexpected behavior you did not resolve.


