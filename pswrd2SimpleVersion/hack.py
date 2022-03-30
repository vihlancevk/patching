i = 0
remains = 32

while True:

    if (i**7 - remains) % 187 == 0:
        break

    i = i + 1

print(i)