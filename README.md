# P2PFileShareApp
A Peer to Peer File Sharing Application.

**Steps To Compile**
$ cd P2P
$ make
This will create create two additional directories:
- obj (for object files)
- bin (contains the final binary: p2p)

**Cleanup:**
$ make clean


*Note:*
- In order to avoid overwrites (when both sender & receiver are using the same directory – bin in my case), I have added a suffix “.rcv” to the downloaded file name.
- It is assumed that the files being transferred are text files.
- It is assumed that the shared text files are present in the same directory which contains the p2p binary.

**Logging:**

A separate log file is created for each instance.
The logfile name is displayed at the beginning of the instance.
Convention Used: log.<Random Number (acts as Device id)>
