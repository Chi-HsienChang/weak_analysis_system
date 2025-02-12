import itertools

fixed = ["111"]
others = ["110", "101", "011", "100", "010", "001", "000"]

def generate_all_combinations_4x1x1x1_with_fixed(fixed, others):
    """
    回傳所有「4+1+1+1=7」的二維結構 (list of list)，
    並把 fixed=["111"] 放在 rank=0。

    範例結構：
        [
            ["111"],               # rank=0 (固定)
            ["110","101","011","100"],   # rank=1 (tie, 4 個)
            ["010"],               # rank=2 (單獨)
            ["001"],               # rank=3 (單獨)
            ["000"],               # rank=4 (單獨)
        ]
    """
    results = []

    # 第一步：從 7 個 others 中，選 4 個作 tie4
    for tie4 in itertools.combinations(others, 4):
        leftover_3 = [x for x in others if x not in tie4]
        # leftover_3 剩 3 個，分別各自成 single
        singles = [[x] for x in leftover_3]  # 例如 [ ["010"], ["001"], ["000"] ]

        # 整理出 4 個群組: [tie4(4), single1, single2, single3]
        groups = [list(tie4)] + singles  # tie4 先轉成 list

        # 第二步：對這 4 組做所有排列 (4! = 24)，對應 rank=1..4
        for perm in itertools.permutations(groups, 4):
            rank_list = [fixed[:]] + list(perm)  # rank=0 放 fixed
            results.append(rank_list)

    return results


def generate_all_combinations_3x3x1_with_fixed(fixed, others):
    """
    回傳所有「3+3+1 = 7」的二維結構 (list of list)，
    並把 fixed=["111"] 放在 rank=0。

    範例結構：
        [
            ["111"],               # rank=0 (固定)
            ["110","101","011"],   # rank=1 (tie, 3 個)
            ["100","010","001"],   # rank=2 (tie, 3 個)
            ["000"],               # rank=3 (單獨 1 個)
        ]
    """
    results = []

    # 第一步：從 7 個 others 中，選 3 個作 tie3a
    for tie3a in itertools.combinations(others, 3):
        leftover_4 = [x for x in others if x not in tie3a]

        # 第二步：從剩下的 4 個中，選 3 個作 tie3b
        for tie3b in itertools.combinations(leftover_4, 3):
            # 剩下 1 個自動成 single1
            leftover_1 = [x for x in leftover_4 if x not in tie3b]

            # 整理出 3 個群組: [tie3a(3), tie3b(3), single1(1)]
            group3a = list(tie3a)
            group3b = list(tie3b)
            single1 = leftover_1  # 只有 1 個

            groups = [group3a, group3b, single1]

            # 第三步：對這 3 組做所有排列 (3! = 6)，對應 rank=1..3
            for perm in itertools.permutations(groups, 3):
                rank_list = [fixed[:]] + list(perm)  # rank=0 放 fixed
                results.append(rank_list)

    return results

def generate_all_combinations_3x2x2_with_fixed(fixed, others):
    """
    回傳所有「3+2+2=7」的二維結構 (list of list)，
    並把 fixed=["111"] 放在 rank=0。

    範例結構：
        [
            ["111"],               # rank=0 (固定)
            ["110","101","011"],   # rank=1 (tie, 3 個)
            ["100","010"],         # rank=2 (tie, 2 個)
            ["001","000"],         # rank=3 (tie, 2 個)
        ]
    """
    results = []

    # 第一步：從 7 個 others 中，選 3 個作 tie3
    for tie3 in itertools.combinations(others, 3):
        leftover_4 = [x for x in others if x not in tie3]

        # 第二步：從剩下的 4 個中，選 2 個作 tie2a
        for tie2a in itertools.combinations(leftover_4, 2):
            tie2b = [x for x in leftover_4 if x not in tie2a]  # 剩下 2 個作 tie2b

            # 整理出 3 個群組: [tie3(3), tie2a(2), tie2b(2)]
            group3 = list(tie3)
            group2a = list(tie2a)
            group2b = tie2b

            groups = [group3, group2a, group2b]

            # 第三步：對這 3 組做所有排列 (3! = 6)，對應 rank=1..3
            for perm in itertools.permutations(groups, 3):
                rank_list = [fixed[:]] + list(perm)  # rank=0 放 fixed
                results.append(rank_list)

    return results



def generate_all_combinations_3x2x1x1_with_fixed(fixed, others):
    """
    回傳所有「3+2+1+1 = 7」的二維結構 (list of list)，
    並把 fixed=["111"] 放在 rank=0。

    範例結構：
        [
            ["111"],               # rank=0 (固定)
            ["110","101","011"],   # rank=1 (tie, 3 個)
            ["100","010"],         # rank=2 (tie, 2 個)
            ["001"],               # rank=3 (單獨)
            ["000"],               # rank=4 (單獨)
        ]
    """
    results = []

    # 第一步：從 7 個 others 中，選 3 個作 tie3
    for tie3 in itertools.combinations(others, 3):
        leftover_4 = [x for x in others if x not in tie3]

        # 第二步：從剩下的 4 個中，選 2 個作 tie2
        for tie2 in itertools.combinations(leftover_4, 2):
            leftover_2 = [x for x in leftover_4 if x not in tie2]

            # leftover_2 是 2 個，各自單獨成一個群組
            single1 = [leftover_2[0]]  # [pattern]
            single2 = [leftover_2[1]]  # [pattern]

            # 整理出 4 個群組: [tie3(3), tie2(2), single1, single2]
            groups = [
                list(tie3),   # 3 個
                list(tie2),   # 2 個
                single1,      # 1 個
                single2,      # 1 個
            ]

            # 第三步：對這 4 組做所有排列 (4! = 24)，對應 rank=1..4
            for perm in itertools.permutations(groups, 4):
                rank_list = [fixed[:]] + list(perm)  # rank=0 放 fixed
                results.append(rank_list)

    return results


def generate_all_combinations_3x1x1x1x1_with_fixed(fixed, others):
    """
    回傳所有「3+1+1+1+1 = 7」的二維結構 (list of list)，
    並把 fixed=["111"] 放在 rank=0。

    範例結構：
        [
            ["111"],               # rank=0 (固定)
            ["110","101","011"],   # rank=1 (tie, 3 個)
            ["100"],               # rank=2 (單獨)
            ["010"],               # rank=3 (單獨)
            ["001"],               # rank=4 (單獨)
            ["000"],               # rank=5 (單獨)
        ]
    """
    results = []

    # 第一步：從 7 個 others 裡選 3 個作 tie3 (3 個並列)
    for tie3 in itertools.combinations(others, 3):
        leftover_4 = [x for x in others if x not in tie3]

        # 現在 tie3 是 3 個，leftover_4 是 4 個
        # 組成群組 => 1 組 3 個 + 4 組各 1 個
        groups = [list(tie3)] + [[x] for x in leftover_4]
        # groups 共有 5 個群組

        # 第二步：對這 5 組做所有排列 (5! = 120 種)
        #         對應到 rank=1..5
        for perm in itertools.permutations(groups, 5):
            rank_list = [fixed[:]] + list(perm)  # rank=0 固定 ["111"]
            results.append(rank_list)

    return results

def generate_all_combinations_2x2x2_with_fixed(fixed, others):
    """
    產生所有「2+2+2+1 = 7」的二維結構，並把 fixed=["111"] 放在 rank=0。

    回傳的資料結構 (list of list) 範例：
        [
            ["111"],               # rank=0 (固定)
            ["110","101"],         # rank=1 (tie, 2 個)
            ["011","100"],         # rank=2 (tie, 2 個)
            ["010","001"],         # rank=3 (tie, 2 個)
            ["000"],               # rank=4 (單獨 1 個)
        ]
    """

    results = []

    # 第一步：從 7 個 others 裡，選 2 個作為 tie1
    for tie1 in itertools.combinations(others, 2):
        leftover_5 = [x for x in others if x not in tie1]

        # 第二步：從剩下的 5 個中，選 2 個作為 tie2
        for tie2 in itertools.combinations(leftover_5, 2):
            leftover_3 = [x for x in leftover_5 if x not in tie2]

            # 第三步：從剩下的 3 個中，選 2 個作為 tie3
            for tie3 in itertools.combinations(leftover_3, 2):
                leftover_1 = [x for x in leftover_3 if x not in tie3]

                # tie1, tie2, tie3 各是 2 個 pattern, leftover_1 是 1 個
                # 整理出 4 組: [tie1(2), tie2(2), tie3(2), single(1)]
                groups = [
                    list(tie1),   # 2 個
                    list(tie2),   # 2 個
                    list(tie3),   # 2 個
                    leftover_1,   # 1 個 (list 只有一個元素)
                ]

                # 對這 4 組做所有排列，對應 rank=1..4
                for perm in itertools.permutations(groups, 4):
                    rank_list = [fixed[:]] + list(perm)  # rank=0 放 fixed
                    results.append(rank_list)

    return results


def generate_all_combinations_2d_with_fixed(fixed, others):
    """
    回傳一個 list，裡面每個元素(稱為 combo_2d) 代表一種「7 個 rank」的配置：
      - combo_2d[0] = ["111"]  (最高 rank 固定)
      - 其餘 combo_2d[1..6] 來自 others，
        且在這 6 個 rank 中，僅有 1 個 rank 含有 2 個 pattern(並列 tie)，
        其餘 5 個 rank 各含 1 個 pattern。
    combo_2d 是「二維結構」(list of list)，例如：
      [
        ["111"],              # rank=0 (最高 rank, fixed)
        ["110"],              # rank=1 (放 1 個 pattern)
        ["001", "101"],       # rank=2 (tie, 放 2 個 pattern)
        ["100"],              # rank=3
        ["010"],              # rank=4
        ["011"],              # rank=5
        ["000"],              # rank=6
      ]
    """
    results = []
    # 從 7 個 others 中，選出「哪 2 個」要放在同一 rank (tie)
    for two_in_same_rank in itertools.combinations(others, 2):
        remaining_5 = [p for p in others if p not in two_in_same_rank]

        # 在 rank=1~6 中挑出一個 rank (tied_rank_index) 放這兩個 pattern
        for tied_rank_index in range(1, 7):
            # 對剩下的 5 個 pattern 排列
            for perm in itertools.permutations(remaining_5):
                # 建立一個 7 個 rank 的二維結構
                rank_list = [[] for _ in range(7)]

                # rank=0 放 fixed（"111"）
                rank_list[0] = fixed[:]

                idx = 0
                for r in range(1, 7):
                    if r == tied_rank_index:
                        # 這個 rank 放 2 個 pattern
                        rank_list[r] = list(two_in_same_rank)
                    else:
                        # 其餘 rank 各放 1 個
                        rank_list[r] = [perm[idx]]
                        idx += 1

                results.append(rank_list)

    return results



def generate_all_combinations_2x2_with_fixed(fixed, others):
    """
    回傳一個 list，裡面每個元素(稱為 combo_2d) 代表：
      - combo_2d[0] = ["111"]  (最高 rank 固定)
      - rank=1..5 (共 5 個 rank)，其中：
          有 2 個 rank：各含 2 個 pattern (tie)
          有 3 個 rank：各含 1 個 pattern
        剛好可以放完 7 個 others。

    combo_2d 是「二維結構」(list of list)，例如：
      [
        ["111"],              # rank=0 (最高 rank, fixed)
        ["110","101"],        # rank=1 (tie, 2個)
        ["011","100"],        # rank=2 (tie, 2個)
        ["010"],              # rank=3 (single)
        ["001"],              # rank=4 (single)
        ["000"],              # rank=5 (single)
      ]
    """
    results = []

    # 第一步：從 7 個 others 中，選 2 個做第一個 tie
    for tie1 in itertools.combinations(others, 2):
        leftover_5 = [x for x in others if x not in tie1]

        # 第二步：再從剩餘的 5 個中，選 2 個做第二個 tie
        for tie2 in itertools.combinations(leftover_5, 2):
            leftover_3 = [x for x in leftover_5 if x not in tie2]

            # tie1, tie2 各是 2 個 pattern，leftover_3 是 3 個 pattern
            # 我們把它們打包成 5 組： [tie1(2), tie2(2), single, single, single]
            # 注意：tie1、tie2 先轉成 list，避免之後要修改或列印
            groups = [
                list(tie1),
                list(tie2),
                [leftover_3[0]],
                [leftover_3[1]],
                [leftover_3[2]],
            ]

            # 第三步：把這 5 組做所有排列，對應到 rank=1..5
            for perm in itertools.permutations(groups, 5):
                # 建立一個二維結構，前面 (rank=0) 放 fixed
                rank_list = [fixed[:]] + list(perm)  # perm 是 5 組
                results.append(rank_list)

    return results


def get_zero1_top_rank_first_bits_2d(rank_list):
    """
    從 rank=0 開始，找第一個出現「中間位=='0'」的 rank，
    並將該 rank 中所有符合 (item[1]=='0') 的 pattern 之「第一位 item[0]」收集成 set。
    """
    for patterns in rank_list:
        zero1_patterns = [p for p in patterns if p[1] == '0']
        if zero1_patterns:
            return set(p[0] for p in zero1_patterns)
    return set()


def get_zero2_top_rank_first_bits_2d(rank_list):
    """
    從 rank=0 開始，找第一個出現「末位=='0'」的 rank，
    並將該 rank 中所有符合 (item[2]=='0') 的 pattern 之「第一位 item[0]」收集成 set。
    """
    for patterns in rank_list:
        zero2_patterns = [p for p in patterns if p[2] == '0']
        if zero2_patterns:
            return set(p[0] for p in zero2_patterns)
    return set()


def get_zero1_zero2_top_rank_first_bits_2d(rank_list):
    """
    從 rank=0 開始，找第一個出現「中間位=='0' AND 末位=='0'」的 rank，
    並將該 rank 中所有符合 (item[1]=='0' & item[2]=='0') 的 pattern 之「第一位 item[0]」收集成 set。
    """
    for patterns in rank_list:
        matched = [p for p in patterns if (p[1] == '0' and p[2] == '0')]
        if matched:
            return set(p[0] for p in matched)
    return set()


def get_zero1_one2_top_rank_first_bits_2d(rank_list):
    """
    從 rank=0 開始，找第一個出現「中間位=='0' AND 末位=='1'」的 rank，
    並將該 rank 中所有符合 (item[1]=='0' & item[2]=='1') 的 pattern 之「第一位 item[0]」收集成 set。
    """
    for patterns in rank_list:
        matched = [p for p in patterns if (p[1] == '0' and p[2] == '1')]
        if matched:
            return set(p[0] for p in matched)
    return set()


def get_one1_zero2_top_rank_first_bits_2d(rank_list):
    """
    從 rank=0 開始，找第一個出現「中間位=='1' AND 末位=='0'」的 rank，
    並將該 rank 中所有符合 (item[1]=='1' & item[2]=='0') 的 pattern 之「第一位 item[0]」收集成 set。
    """
    for patterns in rank_list:
        matched = [p for p in patterns if (p[1] == '1' and p[2] == '0')]
        if matched:
            return set(p[0] for p in matched)
    return set()


def main():
    # 1) 產生所有二維結構，rank=0 固定 ["111"]
    # combo_2d_list = generate_all_combinations_2d_with_fixed(fixed, others)
    # combo_2d_list = generate_all_combinations_2x2_with_fixed(fixed, others)
    combo_2d_list = generate_all_combinations_4x1x1x1_with_fixed(fixed, others)
    print("共有多少組 2D 結構:", len(combo_2d_list))

    # print(combo_2d_list)
    # 2) 示範列出前 3 組
    fit_combinations = []
    for idx, combo_2d in enumerate(combo_2d_list, start=1):
        # print(f"\n=== 2D Combination #{idx} ===")

        # for rank_i, patterns in enumerate(combo_2d):
            # print(f"  rank={rank_i}: {patterns}")

        # (A) 中間位=='0' 的最高 rank
        zero1_set = get_zero1_top_rank_first_bits_2d(combo_2d)
        # print("  => (A) 中間位=='0' 的最高 rank 第一位集合:", zero1_set)

        if zero1_set == set({'1'}):
            zero2_set = get_zero2_top_rank_first_bits_2d(combo_2d)
            if zero2_set == set({'1'}):
                zero1_zero2_set = get_zero1_zero2_top_rank_first_bits_2d(combo_2d) ## 00
                zero1_one2_set = get_zero1_one2_top_rank_first_bits_2d(combo_2d) ## 01
                one1_zero2_set = get_one1_zero2_top_rank_first_bits_2d(combo_2d) ## 10
                if (set({'1'}) != zero1_one2_set and set({'1'}) != one1_zero2_set)\
                    or (zero1_zero2_set != zero1_one2_set and zero1_zero2_set != one1_zero2_set)\
                    or (zero1_one2_set != zero1_zero2_set and zero1_one2_set != set({'1'} ))\
                    or (one1_zero2_set != zero1_zero2_set and one1_zero2_set != set({'1'} )):
                    continue
                else:
                    if zero1_zero2_set != set({'1'}) or zero1_one2_set != set({'1'}) or one1_zero2_set != set({'1'}):
                        fit_combinations.append(combo_2d)
                   


    print(len(fit_combinations))


if __name__ == "__main__":
    main()
