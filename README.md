# pal

print your terminal's 16-color ansi palette as a minimal dot swatch.

## preview

```
  pal :: terminal palette matrix

  ansi    black   red     green   yellow  blue    magenta cyan    white
  dots    ● ○ •   ● ○ •   ● ○ •   ● ○ •   ● ○ •   ● ○ •   ● ○ •   ● ○ •
  hex     #1e1e2e #f38ba8 #a6e3a1 #f9e2af #89b4fa #cba6f7 #89dceb #cdd6f4

  bright  black   red     green   yellow  blue    magenta cyan    white
  dots    ● ○ •   ● ○ •   ● ○ •   ● ○ •   ● ○ •   ● ○ •   ● ○ •   ● ○ •
  hex     #585b70 #f38ba8 #a6e3a1 #f9e2af #89b4fa #cba6f7 #89dceb #a6adc8

  contrast  ● ●    ● ●    ● ●    ● ●    ● ●    ● ●    ● ●    ● ●
            ● ●    ● ●    ● ●    ● ●    ● ●    ● ●    ● ●    ● ●

  syntax  fn main() { println!("hello %s", "world"); // ok }
```

## build

```sh
make
sudo make install
```

or compile directly:

```sh
$(CC) -O2 -std=c99 src/pal.c -o pal
```

## usage

```
pal [options] [view...]
```

### views

- `ansi` — normal 8 ansi colors
- `bright` — bright 8 ansi colors
- `contrast` — contrast check blocks
- `syntax` — code sample
- `mini` — 2-row dot matrix
- `bar` — solid color bar
- `hex` — raw hex list

### options

- `-m, --mini` — compact dots
- `-b, --bar` — color bar
- `-c, --contrast` — contrast blocks
- `-s, --simple` — skip osc 4 queries
- `-p, --plain` — disable ansi escapes (also respects `NO_COLOR`)
- `-v, --version` — print version
- `-h, --help` — print help

## notes

hex codes are queried live from the terminal emulator via osc 4.
works in kitty, alacritty, foot, wezterm, and xterm-compatible emulators.
for dumb terminals or multiplexers blocking osc responses, use `-s`.

## license

mit
