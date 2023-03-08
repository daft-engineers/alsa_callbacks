#include "../inc/callback.hh"
#include <numeric>

int main () {
    alsa_callback::acb test(alsa_callback::format::unsigned_16bit_little_endian);

    auto cb = 
        [](std::vector<uint16_t> vec) {
            // std::cerr << "in callback\n";
            std::cerr << std::accumulate(vec.begin(), vec.end(), decltype(vec)::value_type(0))/vec.size() << "\n";
        };

    std::cerr << "created class" << std::endl;
    test.listen(cb);
    std::cerr << "sleeping for 5 sec\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cerr << "sleeping finished\n";
    test.stop();
}
