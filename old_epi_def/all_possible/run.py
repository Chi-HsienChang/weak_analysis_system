import itertools

#
# 1. 產生所有 4-bit 字串
#
all_4bits = [f"{i:04b}" for i in range(16)]
# ["0000", "0001", "0010", "0011", ..., "1110", "1111"]

# 2. 固定 "1111"；其餘 15 種放在 others
fixed = ["1111"]
others = [b for b in all_4bits if b != "1111"]

#
# 3. 將 fixed 放在最前面，再排列其餘 15 個
#
all_permutations = list(itertools.permutations(others))
sorted_combinations = [fixed + list(p) for p in all_permutations]

#
# 4. 篩選出 fit_combinations
#    (A) 找「排名最高且 item[1] == '0'」的項目, 若 item[0] == '1' ⇒ +1
#    (B) 找「排名最高且 item[2] == '0'」的項目, 若 item[0] == '1' ⇒ +1
#    (C) 找「排名最高且 item[3] == '0'」的項目, 若 item[0] == '1' ⇒ +1
#    若 fit == 3，加入 fit_combinations
#
fit_combinations = []

for combination in sorted_combinations:
    fit = 0

    # (A) 找 item[1] == '0'
    found_item = None
    for item in combination:
        if item[1] == '0':
            found_item = item
            break
    if found_item is not None:
        if found_item[0] == '1':
            fit += 1

    # (B) 找 item[2] == '0'
    found_item = None
    for item in combination:
        if item[2] == '0':
            found_item = item
            break
    if found_item is not None:
        if found_item[0] == '1':
            fit += 1

    # (C) 找 item[3] == '0'
    found_item = None
    for item in combination:
        if item[3] == '0':
            found_item = item
            break
    if found_item is not None:
        if found_item[0] == '1':
            fit += 1

    if fit == 3:
        fit_combinations.append(combination)

print("符合 fit == 3 的組合數量:", len(fit_combinations))

#
# 5. 針對 fit_combinations 中，每個 combination，
#    去找「八種 pattern」(item[1], item[2], item[3]) ∈ {"000","001","010","011","100","101","110","111"}
#    各自排名最前的項目。(rank, item)
#
no_2_epi = []

for combination in fit_combinations:

    # 建立一個字典，記錄該 combination 中：
    #   "000", "001", "010", "011", "100", "101", "110", "111"
    #   這 8 種 pattern 的 (rank, item)
    found_items = {
        "000": None,
        "001": None,
        "010": None,
        "011": None,
        "100": None,
        "101": None,
        "110": None,
        "111": None
    }

    # 在 combination 裡由前到後掃描，最先出現符合 pattern 的就記下來
    for rank, item in enumerate(combination, start=1):
        pattern = item[1] + item[2] + item[3]  # 取第 1,2,3 位
        if pattern in found_items and found_items[pattern] is None:
            found_items[pattern] = (rank, item)

        # 如果 8 種 pattern 都找到了，就可以 break
        if all(v is not None for v in found_items.values()):
            break

    # 若其中有些 pattern 沒被找到（仍是 None），看你要不要篩掉
    # 這裡先示範「如果缺少任何 pattern，就不做後續動作」。
    if not all(v is not None for v in found_items.values()):
        continue

    # （可在這裡做進一步條件判斷、篩選...）
    # 此處就直接把 combination 收進 no_2_epi
    no_2_epi.append((combination, found_items))

#
# 6. 印出最終 no_2_epi 的數量，以及每個 combination 對應的 8 種 pattern (rank, item)
#
print("no_2_epi 數量:", len(no_2_epi))
for idx, (combination, found_items) in enumerate(no_2_epi, start=1):
    print(f"\n=== no_2_epi #{idx} ===")
    print("Combination:", combination)
    # 順手印出 8 種 pattern
    for p in ["000","001","010","011","100","101","110","111"]:
        rank, val = found_items[p]
        print(f"  - pattern {p}, rank={rank}, item={val}")
