#include <dsl/shorthand.h>
#include <filesystem>

namespace AVerifiers {
inline void CheckWaveLength() {
    for (auto [wave, _] : AWave(1_8, 10_18)) {
        const auto& queue = __aOpQueueManager.queues[wave];
        if (queue.waveLength <= 0)
            aLogger->Warning("w{} 未设置波长", wave);
    }
}

inline void CheckCobCD() {
    int baseTime = 0;
    int nCobs = AObjSelector(&APlant::Type, ACOB_CANNON).Count();
    std::deque<std::pair<int, ATime>> cdQueue;
    for (int wave = 0; wave <= __aOpQueueManager.totalWave + 1; ++wave) {
        auto cobOperations = __aOpQueueManager.ExtractOperations<AFunctors::Cob>(wave);
        for (const auto& [time, op] : cobOperations) {
            if (cdQueue.size() == nCobs) {
                auto [prevTime, prevEffectTime] = cdQueue.front();
                cdQueue.pop_front();
                int cd = baseTime + time.time - prevTime;
                if (cd < 3475) {
                    aLogger->Warning("复用不成立 - 上次生效：{}，本次生效：{}，间隔：{}cs",
                        prevEffectTime, time + op.GetEffectOffset(), cd);
                }
            }
            cdQueue.emplace_back(baseTime + time.time, time + op.GetEffectOffset());
        }
        if (__aOpQueueManager.queues[wave].waveLength > 0)
            baseTime += __aOpQueueManager.queues[wave].waveLength;
        else {
            cdQueue.clear();
            baseTime = 0;
        }
    }
}

/*
根据脚本导出 seml 文件。
该功能无法支持阻塞脚本、动态插入操作、非 DSL 算子等情形，导出结果仅供参考。
已知的问题包括：
- 若用卡选择了多个位置，导出的 seml 固定使用第一个位置
*/
inline void ExportSeml() {
    struct WaveEntry {
        std::vector<int> iceTimes;
        int waveLength;
        std::vector<std::pair<int, std::variant<
            AFunctors::Cob,
            AFunctors::Card,
            AFunctors::UseMushroom,
            AFunctors::Fodder
        >>> operations;
    };
    WaveEntry waves[21];
    for (int wave = 1; wave <= 20; ++wave) {
        for (const auto& [time, op] : __aOpQueueManager.ExtractOperations<AFunctors::Cob>(wave)) {
            if (!op.isRecover)
                waves[wave].operations.emplace_back((time + op.GetEffectOffset()).time, op);
        }
        for (const auto& [time, op] : __aOpQueueManager.ExtractOperations<AFunctors::Card>(wave)) {
            waves[wave].operations.emplace_back((time + op.GetEffectOffset()).time, op);
        }
        for (const auto& [time, op] : __aOpQueueManager.ExtractOperations<AFunctors::UseMushroom>(wave)) {
            if (op.seed == AICE_SHROOM)
                waves[wave].iceTimes.push_back((time + op.GetEffectOffset()).time);
            else
                waves[wave].operations.emplace_back((time + op.GetEffectOffset()).time, op);
        }
        for (const auto& [time, op] : __aOpQueueManager.ExtractOperations<AFunctors::Fodder>(wave)) {
            waves[wave].operations.emplace_back((time + op.GetEffectOffset()).time, op);
        }
        for (const auto& [time, op] : __aOpQueueManager.ExtractOperations<AFunctors::CallIceFiller>(wave)) {
            waves[wave].iceTimes.push_back((time + op.GetEffectOffset()).time);
        }
        if (wave % 10 == 9)
            waves[wave].waveLength = 5245;
        else if (wave == 20)
            waves[wave].waveLength = 5999;
        else if (__aOpQueueManager.queues[wave].waveLength > 0)
            waves[wave].waveLength = __aOpQueueManager.queues[wave].waveLength;
        else
            waves[wave].waveLength = 9999;
    }

    std::string outFilename = __SCRIPT__;
    outFilename = outFilename.substr(0, outFilename.find_last_of('.')) + ".seml";
    std::filesystem::path outPath(AStrToWstr(outFilename));
    std::ofstream fout(outPath);

    const std::string_view SCENE_NAMES[] = {"DE", "NE", "PE", "FE", "RE", "ME"};
    fout << std::format("scene:{}\n", SCENE_NAMES[AGetMainObject()->Scene()]);
    std::map<int, int> protectCol;
    for (auto& p : AObjSelector(&APlant::Type, ACOB_CANNON)) {
        if (!protectCol.contains(p.Row() + 1))
            protectCol[p.Row() + 1] = p.Col() + 2;
        else
            protectCol[p.Row() + 1] = std::max(protectCol[p.Row() + 1], p.Col() + 2);
    }
    fout << "protect";
    bool isFirst = true;
    for (const auto& [row, col] : protectCol) {
        fout << (isFirst ? ':' : ' ') << row << col;
        isFirst = false;
    }
    fout << "\navzTime:true\n";
    fout << "repeat:1000\n";
    for (int wave = 1; wave <= 20; ++wave) {
        fout << "\nw" << wave;
        auto& entry = waves[wave];
        std::sort(entry.iceTimes.begin(), entry.iceTimes.end());
        for (int time : entry.iceTimes)
            fout << ' ' << time;
        fout << ' ' << entry.waveLength << '\n';
        for (const auto& [time, op_] : entry.operations) {
            if (auto op = std::get_if<AFunctors::Cob>(&op_)) {
                fout << std::format("P {} {} {}\n", time, op->targetPos.row, op->targetPos.col);
            } else if (auto op = std::get_if<AFunctors::Card>(&op_)) {
                if (op->seed == ADOOM_SHROOM)
                    fout << std::format("N {} {} {}\n", time, op->positions[0].row, op->positions[0].col);
                else if (op->seed == ACHERRY_BOMB)
                    fout << std::format("A {} {} {}\n", time, op->positions[0].row, op->positions[0].col);
                else if (op->seed == AJALAPENO)
                    fout << std::format("J {} {} {}\n", time, op->positions[0].row, op->positions[0].col);
                else if (op->seed == ASQUASH)
                    fout << std::format("W {} {} {}\n", time, op->positions[0].row, op->positions[0].col);
            } else if (auto op = std::get_if<AFunctors::UseMushroom>(&op_)) {
                if (op->seed == ADOOM_SHROOM)
                    fout << std::format("N {} {} {}\n", time, op->positions[0].row, op->positions[0].col);
            } else if (auto op = std::get_if<AFunctors::Fodder>(&op_)) {
                std::map<int, std::vector<int>> colToRows;
                for (const auto& pos : op->positions)
                    colToRows[pos.col].push_back(pos.row);
                for (const auto& [col, rows] : colToRows) {
                    fout << std::format("C {}+{} ", time, op->removalDelay);
                    for (int row : rows)
                        fout << row;
                    fout << std::format(" {}\n", col);
                }
            }
        }
    }
}
} // namespace AVerifiers

AOnAfterScript(
    AVerifiers::CheckWaveLength();
    AVerifiers::CheckCobCD();
    AVerifiers::ExportSeml();
);
