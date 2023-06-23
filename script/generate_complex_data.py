def generate_complex_data(char_set1, char_set2, ratio):
    p1 = ratio
    p2 = 1 - ratio
    data = ‘’
    while len(data) < max_data_length:
        r = random.uniform(0, 1)
        if r < p1:
            char = random.choice(list(char_set1.keys()))
            data += char
        else:
            char = random.choice(list(char_set2.keys()))
            data += char
    return data
