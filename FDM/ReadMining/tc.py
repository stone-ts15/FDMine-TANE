

with open('result.txt') as f:
    lis = []
    for line in f:
        lis.append(line)
    fo = open('tr.txt', 'w')
    for l in sorted(lis):
        fo.write(l)