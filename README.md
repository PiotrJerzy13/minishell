# 🐚 **Minishell**

Welcome to **Minishell**, a lightweight shell implementation inspired by **Bash**, developed as part of the **42 School curriculum**. This project aims to replicate core features of a Unix shell while emphasizing **process management**, **command parsing**, and **signal handling**.

---

## 📚 **Table of Contents**
1. [About the Project](#about-the-project)  
2. [Features](#features)  
3. [Installation](#installation)  
4. [Usage](#usage)  
5. [Built-in Commands](#built-in-commands)  
6. [Project Structure](#project-structure)  
7. [Technical Details](#technical-details)  
8. [Readline Dependency](#readline-dependency)  
9. [Troubleshooting Readline Issues](#troubleshooting-readline-issues)  
10. [Future Improvements](#future-improvements)  

---

## 📝 **About the Project**

**Minishell** is a simplified version of the **Bash shell**. It parses and executes commands entered by the user, supports redirections, pipelines, built-in commands, and respects shell-like behavior in Unix environments.

This project focuses on:
- Understanding **process management** (fork, exec, wait).  
- Parsing commands and handling **pipes** and **redirections**.  
- Implementing **signal handling** (Ctrl+C, Ctrl+Z, Ctrl+D).  
- Managing **environment variables** and shell state.  

---

## 🚀 **Features**

✅ **Interactive Mode:** Input commands via a command prompt.  
✅ **Command Parsing:** Supports multiple commands with pipes (`|`) and redirections (`<`, `>`, `>>`).  
✅ **Built-in Commands:** Includes `echo`, `cd`, `pwd`, `export`, `unset`, `env`, and `exit`.  
✅ **Environment Variables:** Supports `$VARIABLE` expansions.  
✅ **Signal Handling:** Handles signals like `Ctrl+C`, `Ctrl+D`, and `Ctrl+\`.  
✅ **Error Handling:** Graceful error messages for invalid commands or syntax.  
✅ **History:** Command history similar to Bash.

---

## ⚙️ **Installation**

### ✅ **1. Clone the Repository**
```bash
git clone https://github.com/PiotrJerzy13/minishell.git
cd minishell
```

### ✅ **2. Install Readline (if not already installed)**  
#### **On Debian/Ubuntu:**
```bash
sudo apt-get update
sudo apt-get install libreadline-dev
```

#### **On Fedora:**
```bash
sudo dnf install readline-devel
```

#### **On macOS:**
```bash
brew install readline
brew link --force readline
```

### ✅ **3. Build the Project**
```bash
make
```

### ✅ **4. Run the Shell**
```bash
./minishell
```

### ✅ **5. Clean Build Files (optional)**
```bash
make clean
make fclean
```

---

## 🖥️ **Usage**

1. **Start Minishell:**
```bash
./minishell
```

2. **Run Commands:**
```bash
minishell> echo "Hello, World!"
minishell> ls -l | grep minishell
minishell> cat file.txt > output.txt
minishell> export VAR="42"
minishell> echo $VAR
minishell> exit
```

3. **Handle Signals:**
- `Ctrl+C`: Interrupt running command.  
- `Ctrl+D`: Exit shell.  
- `Ctrl+\`: Quit shell.

---

## 📜 **Built-in Commands**

1. **`echo`** - Print arguments to standard output.  
2. **`cd`** - Change directory.  
3. **`pwd`** - Print current working directory.  
4. **`export`** - Set environment variables.  
5. **`unset`** - Remove environment variables.  
6. **`env`** - Print environment variables.  
7. **`exit`** - Exit the shell.  

---

## 🛠️ **Readline Dependency**

Minishell relies on the **`readline` library** to handle user input effectively.

### ✅ **Installing Readline**

**On Debian/Ubuntu:**
```bash
sudo apt-get install libreadline-dev
```

**On macOS:**
```bash
brew install readline
brew link --force readline
```

### ✅ **Makefile Configuration**

Ensure your `Makefile` links with `readline`:

```makefile
LDFLAGS = -lreadline
```

For macOS:
```makefile
LDFLAGS = -L/usr/local/opt/readline/lib -I/usr/local/opt/readline/include -lreadline
```

---

## 🚨 **Troubleshooting Readline Issues**

### ❗ **Issue:** `rl_replace_line` is Undefined

**Error:**
```text
error: call to undeclared function 'rl_replace_line';
```

**Solution:**
1. Ensure `readline` is installed.
2. Update your `Makefile` with `-lreadline`.
3. Rebuild:
```bash
make fclean
make
```

---

## 🚀 **Future Improvements**

Currently, built-in commands and external commands (`execve`) are handled by **two separate logic paths**. This design can lead to code duplication and inconsistency.

### ✅ **Suggested Improvement:**
- **Unify the Execution Logic:**  
   Combine the logic for built-in commands and `execve` into a single execution pipeline.  
- **Benefits of Unification:**  
   - Cleaner and more maintainable code.  
   - Reduced duplication of error handling and command parsing.  
   - Improved scalability for adding new built-in commands.  

This improvement will make the command execution flow more streamlined and robust.

---

### 🚀 **Enjoy Your Minishell Experience!** 🐚✨

Let me know if you want more refinements! 😊
