#include "./util.hpp"

using namespace cc;

float potential_delta(float x) {
    if (x == 0.0f)
        x = uniform_rand(0, 1) ? 0.001f : -0.001f;
    if (0.0f < x && x <  0.001f)
        x =  0.001f;
    if (0.0f > x && x > -0.001f)
        x = -0.001f;
    return 4.0f * std::pow(x, 3.0f);
}

float kernel(float x) {
    return std::exp(-std::pow(std::abs(x), 4.0f));
}

float transition_probability(float l, float r, float step) {
    return std::exp(-std::pow(l - r + step * potential_delta(r), 2.0f) / 4.0f / step);
}

int main(int argc, char *argv[]) {
    try {
        std::vector<float> samples;
        const int          n_loop = 100000;

        const float step = 0.20f;
        float       p    = uniform_rand(-1.0f, 1.0f);
        int accept = 0;
        for (int i = 0; i < n_loop; i++) {
            // 遷移確率を計算する
            float q = p - step * potential_delta(p) + gaussian_rand(0.0f, std::sqrt(2.0f * step));
            float nu = kernel(q) * transition_probability(p, q, step);
            float de = kernel(p) * transition_probability(q, p, step);
            float r  = nu / de;

            if (std::min(1.0f, r) > uniform_rand(0.0f, 1.0f)) {
                accept++;
                p = q;
            }
            samples.push_back(p);
        }

        // 受容率を調べる
        std::cout << format_str("persec: %.1f%%", 100.0f * accept / samples.size()) << std::endl;

        // バーンイン期間を破棄する
        const int n_burnin = samples.size() / 10;
        std::rotate(samples.begin(), samples.begin() + n_burnin, samples.end());
        samples.resize(samples.size() - n_burnin);

        // プロットする
        std::ofstream ofs("output");
        for (auto v : samples) {
            ofs << format_str("%f", v) << std::endl;
        }

        // // カーネルの正規化定数をモンテカルロで求める
        // int count = 0;
        // for (int i = 0; i < 1000000; i++) {
        //     float x = uniform_rand(-5.0f, 5.0f);
        //     float y = uniform_rand(0.0f , 1.0f);
        //     if (y < kernel(x)) count++;
        // }
        // printf("%f", 10.0f * count / 1000000);
    } catch (const std::exception &e) {
        std::cerr << colorant('y', format_str("error: %s", e.what())) << std::endl;
    }
}
