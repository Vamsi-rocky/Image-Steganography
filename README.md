# 🖼️ LSB Image Steganography (Encode & Decode)

This project implements **Image Steganography** using the **Least Significant Bit (LSB)** technique in C language.  
Steganography is the art of hiding secret information inside another file such that it remains invisible to human eyes.

In this project, a **secret text file** is hidden inside a **24-bit BMP image** without changing the visible appearance of the image.  
The hidden data can later be **decoded back** from the stego image.

---

## ✨ Features

| Feature | Description |
|--------|-------------|
| Encode mode | Hides secret data inside a BMP image |
| Decode mode | Extracts hidden data back from the stego image |
| No visible distortion | Uses LSB bit manipulation (least noticeable bit change) |
| Structured & Modular | Clean code divided into modules (encode, decode, main) |
| Works in Linux terminal | Command line usage |

---

## 🧠 How It Works (Concept)

A pixel in a BMP image is stored using **RGB** values (each 1 byte).  
We modify only the **least significant bit (LSB)** of each pixel byte.

Example:

| Original Byte | Secret Bit | Modified Byte |
|--------------|------------|----------------|
| `11001010`   | `1`        | `11001011` ✅ |

Only **one bit** changes → visually indistinguishable.

---

## 🗂️ Project Structure

├── main.c → Controls encode/decode selection
├── encode.c → Logic for hiding (encoding) data
├── decode.c → Logic for extracting (decoding) data
├── encode.h → Function declarations for encoding
├── decode.h → Function declarations for decoding
├── types.h → Custom type definitions (Status, OperationType)
├── common.h → Contains MAGIC_STRING declaration

