#include <iostream>
#include <atomic>
#include <cstring>  // for memcpy
#include <thread>   // for std::thread
#include <array>    // FIX 4: Included array

// 1. Corrected Struct
struct can_message {
    uint32_t can_id;
    uint8_t  dlc;
    uint8_t  can_data[8]; 
};

// 2. Fixed Ringbuffer
class Ringbuffer {
private:
    const int Size = 1024; 
    std::array<can_message, 1024> buffer; 
    
    std::atomic<int> write_idx{0};
    std::atomic<int> read_idx{0};

public:
    bool store(const can_message& msg) {
        int next_write = (write_idx + 1) % Size;
        if (next_write == read_idx) return false; 
        
        buffer[write_idx] = msg;
        write_idx = next_write;
        return true;
    }

    bool get(can_message& msg) {
        if (read_idx == write_idx) return false; 
        
        msg = buffer[read_idx];
        read_idx = (read_idx + 1) % Size;
        return true;
    }
};

// FIX 1: Removed the empty () to avoid the "Most Vexing Parse"
Ringbuffer myRingBuffer;

// 3. Deserializer (The Producer)
void upd_can(uint8_t* buffer, int len) {
    can_message message;
    
    // Great use of pointer math!
    std::memcpy(&message.can_id, buffer + 24, 4);
    message.can_id &= 0x7FF; 
    
    // FIX 2: Correct order of operations for pointer math, 
    // or just use buffer[21]
    message.dlc = *(buffer + 21); 
    
    // Great use of pointer math!
    std::memcpy(message.can_data, buffer + 28, 8);
    
    myRingBuffer.store(message);
}

// 4. Consumer Thread
void send_can() {
    can_message msg_to_send;
    while(true) {
        if (myRingBuffer.get(msg_to_send)) {
            std::cout << "Sending CAN ID: " << msg_to_send.can_id << "\n";
        }
    }
}

// 5. Main Loop
int main() {
    std::cout << "Gateway Started\n";
    
    std::thread consumer_thread(send_can);
    
    uint8_t buffer[64];
    
    // Fake UDP packet setup to prove it works
    // Setting byte 24 to 0x05 so the CAN ID becomes 5
    buffer[24] = 0x05; 
    
    int loops = 0;
    while(loops < 3) { // Running 3 times to prove it works
        int len = 36; 
        if (len > 0) {
            upd_can(buffer, len);
        }
        loops++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // slow it down for the terminal
    }
    
    // In a real program, we'd need a way to stop the infinite thread safely,
    // but for this example, we'll just detach it or exit.
    consumer_thread.detach(); 
    return 0;
}