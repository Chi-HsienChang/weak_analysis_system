import itertools

fixed = ["111"]
others = ["110", "101", "011", "100", "010", "001", "000"]

all_permutations = list(itertools.permutations(others))
sorted_combinations = [fixed + list(permutation) for permutation in all_permutations]

fit_combinations = []

# 只查看前 1 組 combination（也就是第 0 組），以示範為主
for combination_idx, combination in enumerate(sorted_combinations, start=1):
    fit = 0
    # print(f"=== Combination #{combination_idx} ===")

    found_item = None  # 用於紀錄「排名最高且 item[1] == '0'」的字串
    for item in combination:        
        # 檢查 item[1] 是否為 '0'
        if item[1] == '0':
            found_item = item
            # 找到第一個符合的就 break，確保這個就是「排名最高」的
            break

    if found_item is not None:
        # 判斷 item[0]：決定「1 指向 0」或「1 不指向 0」
        if found_item[0] == '0':
            fit += 0
            # print(f"1 指向 0")
        else:
            fit += 1
            # print(f"1 不指向 0")

    found_item = None  # 用於紀錄「排名最高且 item[1] == '0'」的字串
    for item in combination:        
        # 檢查 item[1] 是否為 '0'
        if item[2] == '0':
            found_item = item
            # 找到第一個符合的就 break，確保這個就是「排名最高」的
            break

    if found_item is not None:
        # 判斷 item[0]：決定「1 指向 0」或「1 不指向 0」
        if found_item[0] == '0':
            fit += 0
            # print(f"1 指向 0")
        else:
            fit += 1
            # print(f"1 不指向 0")

    if fit == 2:
        fit_combinations = fit_combinations + [combination]



print(len(fit_combinations))
# 假設這裡直接使用 sorted_combinations（或 fit_combinations）做示範
# 若你原本有篩選步驟 (fit == 2)，就把下方的程式改成使用 fit_combinations 即可
no_2_epi = []

for combination_idx, combination in enumerate(fit_combinations, start=1):

    # 建立一個字典，用來記錄在該 combination 中
    # "00", "01", "10", "11" 四種 pattern 的「第一個(最高排名)」(rank, item)
    found_items = {
        "00": None,
        "01": None,
        "10": None,
        "11": None
    }

    # 依排名順序遍歷每個 item
    for rank, item in enumerate(combination, start=1):
        pattern = item[1] + item[2]  # 只看中間位與末位
        if found_items[pattern] is None:
            found_items[pattern] = (rank, item)

        # 如果四種 pattern 都已經找到，就可以提前結束
        if all(x is not None for x in found_items.values()):
            break

    # 先檢查是否四種 pattern 都有找到
    if not all(x is not None for x in found_items.values()):
        # 若有任一為 None，直接跳過或看你要怎麼處理
        continue

    # 取得四種 pattern 對應的 (rank, item)
    item_00 = found_items["00"]  # (rank, "XYZ")
    item_01 = found_items["01"]
    item_10 = found_items["10"]
    item_11 = found_items["11"]

    if (
        (item_00[1][0] != item_10[1][0] and item_00[1][0] != item_01[1][0])
        or (item_11[1][0] != item_10[1][0] and item_11[1][0] != item_01[1][0])
        or (item_01[1][0] != item_11[1][0] and item_01[1][0] != item_00[1][0])
        or (item_10[1][0] != item_11[1][0] and item_10[1][0] != item_00[1][0])
    ):
        # 你的條件符合
        pass
    else:
        no_2_epi.append(combination)

print("=== 結果 no_2_epi 的筆數:", len(no_2_epi))


for combination_idx, combination in enumerate(no_2_epi, start=1):

    # 建立一個字典，用來記錄在該 combination 中
    # "00", "01", "10", "11" 四種 pattern 的「第一個(最高排名)」(rank, item)
    found_items = {
        "00": None,
        "01": None,
        "10": None,
        "11": None
    }

    # 依排名順序遍歷每個 item
    for rank, item in enumerate(combination, start=1):
        pattern = item[1] + item[2]  # 只看中間位與末位
        if found_items[pattern] is None:
            found_items[pattern] = (rank, item)

        # 如果四種 pattern 都已經找到，就可以提前結束
        if all(x is not None for x in found_items.values()):
            break

    # 先檢查是否四種 pattern 都有找到
    if not all(x is not None for x in found_items.values()):
        # 若有任一為 None，直接跳過或看你要怎麼處理
        continue

    # 取得四種 pattern 對應的 (rank, item)
    item_00 = found_items["00"]  # (rank, "XYZ")
    item_01 = found_items["01"]
    item_10 = found_items["10"]
    item_11 = found_items["11"]

    # ------------------------
    #  印出 rank
    # ------------------------
    print(f"=== Combination #{combination_idx} ===")
    print("  - 00 rank:", item_00[0], " item:", item_00[1])
    print("  - 01 rank:", item_01[0], " item:", item_01[1])
    print("  - 10 rank:", item_10[0], " item:", item_10[1])
    print("  - 11 rank:", item_11[0], " item:", item_11[1])