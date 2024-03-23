with open("numbers", "rb") as file:
    byte_data = file.read()

numbers = [byte for byte in byte_data]

print(numbers)
