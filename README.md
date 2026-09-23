# Lingrui Bomb Lab

*Adapted from the CS:APP Bomb Lab (Carnegie Mellon University). Featuring Evil EarthCloud as the villain.*

**THE EVIL EARTHCLOUD WILL BLOW YOU GUYS UP**

**Note:** this lab is in English on purpose.

## 1. Introduction

The nefarious **Evil EarthCloud** has planted a binary bomb on your machine.

He says, "Hina Youmiya is the greatest female voice actor ever. If you don't agree, I will blow up your computer."

What a crazy guy. You'd better defuse the bomb — or your computer will be destroyed.

A *binary bomb* is a program that consists of a sequence of **five phases**. Each phase expects you
to type a particular string on `stdin`. If you type the correct string, then the phase is **defused**
and the bomb proceeds to the next phase. Otherwise, the bomb **explodes** by printing `BOOM!!!` and
then terminating. The bomb is defused when every phase has been defused, at which point it prints:

```text
Congratulations! You've defused the bomb!
```

**Your mission, which you have no choice but to accept, is to defuse your bomb before the due date.**
Good luck, and welcome to the bomb squad.

> ⚠️ **Warning: do not try to defuse the bomb by brute force.** The bomb is not designed to be
> brute-forced, and you have better things to do with your CPU cycles. You are expected to *read the
> assembly* and to *use a debugger* — that is the whole point of this lab.

---

## 2. Logistics

### 2.1 What you are given

| File | Description |
| --- | --- |
| `bomb` | The bomb itself: an x86-64 Linux executable. **Do not modify it.** |
| `bomb.c` | The only source code you get — just `main`, with **no** phase logic. |
| `solution.txt` | Where your answers go: **one line per phase, five lines total**. |
| `bomb.sha256` | Checksum used by the grader to detect a tampered binary. |
| `tools/autograde.sh` | The autograder. The CI runs exactly this script. |
| `.github/workflows/autograde.yml` | GitHub Actions workflow that triggers the autograder. |

> Setup instructions, a debugging guide and phase hints live in the assignment document.

### 2.2 How to hand in

1. Put your answers in `solution.txt`, commit, and push to your fork.
2. Open the **Actions** tab → select `Autograde` → **Run workflow**.
3. Open the finished run: the grading report is in the **Summary** section at the bottom
   (and also available as the `autograde-report` artifact).

You should also run the grader locally — it takes a second:

```bash
bash tools/autograde.sh              # grades solution.txt
bash tools/autograde.sh other.txt    # grades another file
```

---

## 3. Getting Started

The bomb is a **Linux x86-64 ELF executable**: you need Linux, or Windows + **WSL2**.

```bash
chmod +x bomb

# (1) Type something silly and watch it blow up — build some respect for the bomb
./bomb
> test
...
BOOM!!!
The bomb has blown up.

# (2) Feed it a file: the bomb reads lines from the file, then falls back to stdin.
#     This is how you work: as you defuse each phase, append its answer to the file.
./bomb solution.txt
```

`solution.txt` starts out with five `???` placeholder lines. Replace them one by one as you defuse
each phase — that way you never have to retype the earlier answers.

### 3.1 Single-phase mode

The bomb can also run **one phase in isolation**, which is convenient when you are stuck on a
particular phase (or when you want to check your answer without replaying the whole chain):

```bash
./bomb --phase 3              # read one line from stdin, test phase 3 only
./bomb --phase 3 line3.txt    # ... or read it from a file
# pass -> [phase 3] PASS   (exit code 0)
# fail -> [phase 3] FAILED (exit code 1)
```

The classic chained mode is unchanged: `./bomb solution.txt` runs all five phases in sequence and
terminates at the first explosion.

---

## 4. Grading

| Item | Rule |
| --- | --- |
| Points | **20 points per phase**, 100 total |
| Per-phase grading | Each phase is graded **on its own** (`./bomb --phase N`). Failing phase 3 costs you nothing on phases 4 and 5 |
| Binary integrity | `bomb` is checksummed (`bomb.sha256`) before grading. A modified binary scores **0** |
| Report | A per-phase ✅/❌ table, written to the Actions Summary and uploaded as an artifact |

The grader fails the CI run (red ❌) until all five phases are defused. A partially solved bomb
still produces a report showing exactly which phases are done.

---

## 5. References

- *Computer Systems: A Programmer's Perspective* (CS:APP), Chapter 3 — the theory behind this lab.
- The CS:APP Bomb Lab writeup (Carnegie Mellon University) — the original assignment this is based on.
