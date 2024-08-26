import argparse
import numpy as np
import cv2

WIDTH = 10
# make input image texture seamless


def main(args):
    input_image = cv2.imread(args.input)
    h, w, _ = input_image.shape
    half_h = h // 2
    half_w = w // 2
    tmp_image = np.copy(input_image)

    # split image into 4 parts and reassemble them
    tmp_image[:half_h, :half_w] = input_image[half_h:, half_w:]
    tmp_image[:half_h, half_w:] = input_image[half_h:, :half_w]
    tmp_image[half_h:, :half_w] = input_image[:half_h, half_w:]
    tmp_image[half_h:, half_w:] = input_image[:half_h, :half_w]

    # poly
    mask = np.zeros((h, w), dtype=np.uint8)
    poly = np.array(
        [
            [half_w - WIDTH, 0],
            [half_w + WIDTH, 0],
            [half_w + WIDTH, half_h - WIDTH],
            [w, half_h - WIDTH],
            [w, half_h + WIDTH],
            [half_w + WIDTH, half_h + WIDTH],
            [half_w + WIDTH, h],
            [half_w - WIDTH, h],
            [half_w - WIDTH, half_h + WIDTH],
            [0, half_h + WIDTH],
            [0, half_h - WIDTH],
            [half_w - WIDTH, half_h - WIDTH],
        ]
    )
    cv2.fillPoly(mask, [poly], 255)

    # apply mask
    blurred = cv2.GaussianBlur(tmp_image, (WIDTH * 2 + 1, WIDTH * 2 + 1), 0)
    tmp_image = np.where(mask[..., None] == 255, blurred, tmp_image)

    # move the order of the image back

    output_image = np.copy(tmp_image)

    output_image[:half_h, :half_w] = tmp_image[half_h:, half_w:]
    output_image[:half_h, half_w:] = tmp_image[half_h:, :half_w]
    output_image[half_h:, :half_w] = tmp_image[:half_h, half_w:]
    output_image[half_h:, half_w:] = tmp_image[:half_h, :half_w]

    cv2.imwrite(f"{args.input}_seamless.png", output_image)


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("input", help="input image file")

    args = arg_parser.parse_args()
    main(args)
