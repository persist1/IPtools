#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
GitHub 自动化助手 Pro - 增强版
支持配置管理、多种认证方式、智能触发构建
"""

import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox, simpledialog
import subprocess
import threading
import os
import sys
import webbrowser
import json
from datetime import datetime

class ConfigDialog:
    """配置对话框"""
    def __init__(self, parent, current_config=None):
        self.result = None
        self.dialog = tk.Toplevel(parent)
        self.dialog.title("⚙️ 项目配置")
        self.dialog.geometry("500x400")
        self.dialog.transient(parent)
        self.dialog.grab_set()
        
        # 居中显示
        self.dialog.update_idletasks()
        x = (self.dialog.winfo_screenwidth() // 2) - 250
        y = (self.dialog.winfo_screenheight() // 2) - 200
        self.dialog.geometry(f"500x400+{x}+{y}")
        
        # 当前配置
        self.config = current_config or {}
        
        self.create_widgets()
    
    def create_widgets(self):
        """创建配置界面"""
        main_frame = tk.Frame(self.dialog, padx=20, pady=20)
        main_frame.pack(fill=tk.BOTH, expand=True)
        
        # 标题
        title = tk.Label(
            main_frame,
            text="📝 配置 GitHub 项目信息",
            font=("Arial", 14, "bold")
        )
        title.pack(pady=(0, 20))
        
        # 仓库地址
        tk.Label(main_frame, text="GitHub 仓库地址:", font=("Arial", 10, "bold")).pack(anchor=tk.W)
        tk.Label(main_frame, text="例如: https://github.com/username/repo.git", font=("Arial", 8), fg="gray").pack(anchor=tk.W)
        self.repo_entry = tk.Entry(main_frame, font=("Arial", 10), width=50)
        self.repo_entry.pack(pady=(5, 15), fill=tk.X)
        self.repo_entry.insert(0, self.config.get("repo_url", ""))
        
        # 用户名
        tk.Label(main_frame, text="GitHub 用户名:", font=("Arial", 10, "bold")).pack(anchor=tk.W)
        self.username_entry = tk.Entry(main_frame, font=("Arial", 10), width=50)
        self.username_entry.pack(pady=(5, 15), fill=tk.X)
        self.username_entry.insert(0, self.config.get("username", ""))
        
        # 邮箱
        tk.Label(main_frame, text="Git 邮箱:", font=("Arial", 10, "bold")).pack(anchor=tk.W)
        self.email_entry = tk.Entry(main_frame, font=("Arial", 10), width=50)
        self.email_entry.pack(pady=(5, 15), fill=tk.X)
        self.email_entry.insert(0, self.config.get("email", ""))
        
        # 认证方式选择
        auth_frame = tk.Frame(main_frame)
        auth_frame.pack(fill=tk.X, pady=(5, 15))
        
        tk.Label(auth_frame, text="认证方式:", font=("Arial", 10, "bold")).pack(anchor=tk.W)
        
        self.auth_type = tk.StringVar(value=self.config.get("auth_type", "password"))
        
        tk.Radiobutton(
            auth_frame,
            text="密码认证（每次输入）",
            variable=self.auth_type,
            value="password",
            font=("Arial", 9),
            command=self.update_auth_fields
        ).pack(anchor=tk.W)
        
        tk.Radiobutton(
            auth_frame,
            text="Personal Access Token（推荐）",
            variable=self.auth_type,
            value="token",
            font=("Arial", 9),
            command=self.update_auth_fields
        ).pack(anchor=tk.W)
        
        # Token输入框
        self.token_frame = tk.Frame(main_frame)
        self.token_frame.pack(fill=tk.X, pady=(5, 15))
        
        tk.Label(self.token_frame, text="Personal Access Token:", font=("Arial", 10)).pack(anchor=tk.W)
        tk.Label(
            self.token_frame,
            text="生成地址: https://github.com/settings/tokens (需要 repo 权限)",
            font=("Arial", 8),
            fg="blue",
            cursor="hand2"
        ).pack(anchor=tk.W)
        
        self.token_entry = tk.Entry(self.token_frame, font=("Arial", 10), width=50, show="*")
        self.token_entry.pack(pady=(5, 0), fill=tk.X)
        self.token_entry.insert(0, self.config.get("token", ""))
        
        # 显示/隐藏token
        self.show_token_var = tk.BooleanVar()
        tk.Checkbutton(
            self.token_frame,
            text="显示 Token",
            variable=self.show_token_var,
            command=self.toggle_token_visibility
        ).pack(anchor=tk.W)
        
        self.update_auth_fields()
        
        # 按钮
        btn_frame = tk.Frame(main_frame)
        btn_frame.pack(pady=20)
        
        tk.Button(
            btn_frame,
            text="💾 保存配置",
            command=self.save_config,
            bg="#2ecc71",
            fg="white",
            font=("Arial", 11, "bold"),
            width=12,
            padx=10,
            pady=5
        ).pack(side=tk.LEFT, padx=5)
        
        tk.Button(
            btn_frame,
            text="❌ 取消",
            command=self.dialog.destroy,
            bg="#95a5a6",
            fg="white",
            font=("Arial", 11, "bold"),
            width=12,
            padx=10,
            pady=5
        ).pack(side=tk.LEFT, padx=5)
    
    def update_auth_fields(self):
        """更新认证字段显示"""
        if self.auth_type.get() == "token":
            self.token_frame.pack(fill=tk.X, pady=(5, 15))
        else:
            self.token_frame.pack_forget()
    
    def toggle_token_visibility(self):
        """切换Token显示/隐藏"""
        if self.show_token_var.get():
            self.token_entry.config(show="")
        else:
            self.token_entry.config(show="*")
    
    def save_config(self):
        """保存配置"""
        repo_url = self.repo_entry.get().strip()
        username = self.username_entry.get().strip()
        email = self.email_entry.get().strip()
        auth_type = self.auth_type.get()
        token = self.token_entry.get().strip() if auth_type == "token" else ""
        
        # 验证必填项
        if not repo_url:
            messagebox.showwarning("配置错误", "请输入仓库地址!")
            return
        
        if not username:
            messagebox.showwarning("配置错误", "请输入用户名!")
            return
        
        if not email:
            messagebox.showwarning("配置错误", "请输入邮箱!")
            return
        
        if auth_type == "token" and not token:
            messagebox.showwarning("配置错误", "请输入 Personal Access Token!")
            return
        
        # 保存配置
        self.result = {
            "repo_url": repo_url,
            "username": username,
            "email": email,
            "auth_type": auth_type,
            "token": token
        }
        
        self.dialog.destroy()
    
    def show(self):
        """显示对话框并返回结果"""
        self.dialog.wait_window()
        return self.result


class GitHubHelper:
    def __init__(self, root):
        self.root = root
        self.root.title("GitHub 自动化助手 Pro")
        self.root.geometry("850x650")
        self.root.resizable(True, True)
        
        # 配置文件路径
        self.config_file = "github_config.json"
        
        # 加载配置
        self.config = self.load_config()
        
        # 创建界面
        self.create_widgets()
        
        # 检查Git和配置
        self.check_environment()
    
    def load_config(self):
        """加载配置"""
        if os.path.exists(self.config_file):
            try:
                with open(self.config_file, "r", encoding="utf-8") as f:
                    return json.load(f)
            except:
                return {}
        return {}
    
    def save_config(self):
        """保存配置"""
        try:
            with open(self.config_file, "w", encoding="utf-8") as f:
                json.dump(self.config, f, indent=4, ensure_ascii=False)
            return True
        except Exception as e:
            messagebox.showerror("保存失败", f"配置保存失败: {str(e)}")
            return False
    
    def create_widgets(self):
        """创建界面组件"""
        
        # 标题
        title_frame = tk.Frame(self.root, bg="#2c3e50", height=60)
        title_frame.pack(fill=tk.X)
        title_frame.pack_propagate(False)
        
        title_label = tk.Label(
            title_frame,
            text="🚀 GitHub 自动化助手 Pro",
            font=("Arial", 20, "bold"),
            bg="#2c3e50",
            fg="white"
        )
        title_label.pack(side=tk.LEFT, padx=20, pady=15)
        
        # 配置按钮
        config_btn = tk.Button(
            title_frame,
            text="⚙️ 配置",
            command=self.show_config_dialog,
            bg="#3498db",
            fg="white",
            font=("Arial", 11, "bold"),
            padx=15,
            pady=5
        )
        config_btn.pack(side=tk.RIGHT, padx=20, pady=15)
        
        # 主容器
        main_frame = tk.Frame(self.root, padx=20, pady=20)
        main_frame.pack(fill=tk.BOTH, expand=True)
        
        # 项目信息
        info_frame = tk.LabelFrame(main_frame, text="📦 当前项目", font=("Arial", 12, "bold"), padx=10, pady=10)
        info_frame.pack(fill=tk.X, pady=(0, 10))
        
        self.info_label = tk.Label(info_frame, text="请先配置项目信息", font=("Arial", 10), fg="orange")
        self.info_label.pack(anchor=tk.W)
        
        self.update_info_display()
        
        # 按钮区域
        button_frame = tk.LabelFrame(main_frame, text="🎯 快速操作", font=("Arial", 12, "bold"), padx=10, pady=10)
        button_frame.pack(fill=tk.X, pady=(0, 10))
        
        # 第一行按钮
        row1 = tk.Frame(button_frame)
        row1.pack(fill=tk.X, pady=5)
        
        self.btn_upload = tk.Button(
            row1,
            text="📤 一键上传到 GitHub",
            command=self.upload_to_github,
            bg="#3498db",
            fg="white",
            font=("Arial", 11, "bold"),
            padx=10,
            pady=10,
            width=25
        )
        self.btn_upload.pack(side=tk.LEFT, padx=5)
        
        self.btn_trigger = tk.Button(
            row1,
            text="🔨 触发自动构建",
            command=self.trigger_build,
            bg="#9b59b6",
            fg="white",
            font=("Arial", 11, "bold"),
            padx=10,
            pady=10,
            width=25
        )
        self.btn_trigger.pack(side=tk.LEFT, padx=5)
        
        # 第二行按钮
        row2 = tk.Frame(button_frame)
        row2.pack(fill=tk.X, pady=5)
        
        self.btn_view = tk.Button(
            row2,
            text="👀 查看构建状态",
            command=self.view_actions,
            bg="#1abc9c",
            fg="white",
            font=("Arial", 11, "bold"),
            padx=10,
            pady=10,
            width=25
        )
        self.btn_view.pack(side=tk.LEFT, padx=5)
        
        self.btn_release = tk.Button(
            row2,
            text="🏷️ 创建发布版本",
            command=self.create_release,
            bg="#e74c3c",
            fg="white",
            font=("Arial", 11, "bold"),
            padx=10,
            pady=10,
            width=25
        )
        self.btn_release.pack(side=tk.LEFT, padx=5)
        
        # 第三行按钮
        row3 = tk.Frame(button_frame)
        row3.pack(fill=tk.X, pady=5)
        
        self.btn_open_repo = tk.Button(
            row3,
            text="🌐 打开 GitHub 仓库",
            command=self.open_repo,
            bg="#34495e",
            fg="white",
            font=("Arial", 11, "bold"),
            padx=10,
            pady=10,
            width=25
        )
        self.btn_open_repo.pack(side=tk.LEFT, padx=5)
        
        self.btn_enable_actions = tk.Button(
            row3,
            text="✅ 启用 GitHub Actions",
            command=self.enable_actions,
            bg="#f39c12",
            fg="white",
            font=("Arial", 11, "bold"),
            padx=10,
            pady=10,
            width=25
        )
        self.btn_enable_actions.pack(side=tk.LEFT, padx=5)
        
        # 日志区域
        log_frame = tk.LabelFrame(main_frame, text="📋 操作日志", font=("Arial", 12, "bold"), padx=10, pady=10)
        log_frame.pack(fill=tk.BOTH, expand=True)
        
        self.log_text = scrolledtext.ScrolledText(
            log_frame,
            font=("Consolas", 9),
            bg="#2c3e50",
            fg="#ecf0f1",
            insertbackground="white",
            wrap=tk.WORD
        )
        self.log_text.pack(fill=tk.BOTH, expand=True)
        
        # 状态栏
        self.status_bar = tk.Label(
            self.root,
            text="准备就绪",
            bd=1,
            relief=tk.SUNKEN,
            anchor=tk.W,
            font=("Arial", 9)
        )
        self.status_bar.pack(side=tk.BOTTOM, fill=tk.X)
    
    def update_info_display(self):
        """更新项目信息显示"""
        if self.config:
            repo_url = self.config.get("repo_url", "未配置")
            username = self.config.get("username", "未配置")
            auth_type = self.config.get("auth_type", "password")
            auth_text = "Token认证" if auth_type == "token" else "密码认证"
            
            info_text = f"仓库: {repo_url}\n用户: {username}\n认证: {auth_text}"
            self.info_label.config(text=info_text, fg="green")
        else:
            self.info_label.config(text="⚠️ 请点击右上角'配置'按钮设置项目信息", fg="orange")
    
    def show_config_dialog(self):
        """显示配置对话框"""
        dialog = ConfigDialog(self.root, self.config)
        result = dialog.show()
        
        if result:
            self.config = result
            if self.save_config():
                self.log("配置已保存", "SUCCESS")
                self.update_info_display()
                messagebox.showinfo("配置成功", "项目配置已保存!")
    
    def check_environment(self):
        """检查环境"""
        # 检查配置
        if not self.config:
            self.log("⚠️ 请先配置项目信息", "WARNING")
            self.log("点击右上角 '配置' 按钮进行设置", "INFO")
        
        # 检查Git
        self.log("检查 Git 安装状态...")
        success, output, error = self.run_command("git --version")
        
        if success:
            version = output.strip()
            self.log(f"Git 已安装: {version}", "SUCCESS")
            self.update_status(f"Git 已就绪 - {version}")
        else:
            self.log("Git 未安装或未添加到 PATH", "ERROR")
            self.update_status("错误: Git 未安装")
            messagebox.showerror(
                "Git 未安装",
                "请先安装 Git!\n\n下载地址: https://git-scm.com/download/win"
            )
    
    def log(self, message, level="INFO"):
        """写入日志"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        
        # 颜色标记
        if level == "SUCCESS":
            prefix = "✓"
        elif level == "ERROR":
            prefix = "✗"
        elif level == "WARNING":
            prefix = "⚠"
        else:
            prefix = "ℹ"
        
        log_entry = f"[{timestamp}] {prefix} {message}\n"
        
        self.log_text.insert(tk.END, log_entry)
        self.log_text.see(tk.END)
        self.root.update()
    
    def update_status(self, text):
        """更新状态栏"""
        self.status_bar.config(text=text)
        self.root.update()
    
    def run_command(self, command, cwd=None, input_text=None):
        """运行命令"""
        try:
            result = subprocess.run(
                command,
                cwd=cwd or os.getcwd(),
                shell=True,
                capture_output=True,
                text=True,
                encoding='utf-8',
                errors='ignore',
                input=input_text
            )
            return result.returncode == 0, result.stdout, result.stderr
        except Exception as e:
            return False, "", str(e)
    
    def get_auth_url(self):
        """获取带认证的仓库URL"""
        if not self.config:
            return None
        
        repo_url = self.config.get("repo_url", "")
        auth_type = self.config.get("auth_type", "password")
        
        if auth_type == "token":
            token = self.config.get("token", "")
            if token and repo_url.startswith("https://github.com/"):
                # 将 https://github.com/user/repo.git 转换为 https://token@github.com/user/repo.git
                auth_url = repo_url.replace("https://", f"https://{token}@")
                return auth_url
        
        return repo_url
    
    def disable_buttons(self):
        """禁用所有按钮"""
        self.btn_upload.config(state=tk.DISABLED)
        self.btn_trigger.config(state=tk.DISABLED)
        self.btn_view.config(state=tk.DISABLED)
        self.btn_release.config(state=tk.DISABLED)
        self.btn_open_repo.config(state=tk.DISABLED)
        self.btn_enable_actions.config(state=tk.DISABLED)
    
    def enable_buttons(self):
        """启用所有按钮"""
        self.btn_upload.config(state=tk.NORMAL)
        self.btn_trigger.config(state=tk.NORMAL)
        self.btn_view.config(state=tk.NORMAL)
        self.btn_release.config(state=tk.NORMAL)
        self.btn_open_repo.config(state=tk.NORMAL)
        self.btn_enable_actions.config(state=tk.NORMAL)
    
    def upload_to_github(self):
        """上传到GitHub"""
        if not self.config:
            messagebox.showwarning("未配置", "请先配置项目信息!")
            self.show_config_dialog()
            return
        
        def task():
            self.disable_buttons()
            self.update_status("正在上传到 GitHub...")
            
            try:
                self.log("=" * 50)
                self.log("开始上传到 GitHub", "INFO")
                self.log("=" * 50)
                
                username = self.config.get("username")
                email = self.config.get("email")
                repo_url = self.config.get("repo_url")
                auth_url = self.get_auth_url()
                
                # 配置Git
                self.log("[1/6] 配置 Git 用户信息...")
                self.run_command(f'git config user.name "{username}"')
                self.run_command(f'git config user.email "{email}"')
                self.log("配置完成", "SUCCESS")
                
                # 初始化
                self.log("[2/6] 初始化 Git 仓库...")
                if not os.path.exists(".git"):
                    self.run_command("git init")
                    self.log("初始化完成", "SUCCESS")
                else:
                    self.log("仓库已存在", "SUCCESS")
                
                # 配置远程仓库
                self.log("[3/6] 配置远程仓库...")
                self.run_command("git remote remove origin")
                
                # 根据认证方式使用不同的URL
                if self.config.get("auth_type") == "token":
                    self.run_command(f'git remote add origin {auth_url}')
                    self.log("使用 Token 认证", "INFO")
                else:
                    self.run_command(f'git remote add origin {repo_url}')
                    self.log("使用密码认证（需要手动输入）", "INFO")
                
                self.log("远程仓库已配置", "SUCCESS")
                
                # 添加文件
                self.log("[4/6] 添加文件...")
                self.run_command("git add .")
                self.log("文件已添加", "SUCCESS")
                
                # 提交
                self.log("[5/6] 提交更改...")
                success, _, _ = self.run_command('git commit -m "Update: IPtools project with GitHub Actions"')
                if success:
                    self.log("提交完成", "SUCCESS")
                else:
                    self.log("没有新的更改", "WARNING")
                    self.run_command('git commit --allow-empty -m "Initial commit"')
                
                # 推送
                self.log("[6/6] 推送到 GitHub...")
                if self.config.get("auth_type") == "password":
                    self.log("⚠️ 请在弹出窗口输入密码...", "WARNING")
                
                self.run_command("git branch -M main")
                success, output, error = self.run_command("git push -u origin main")
                
                if success or "Everything up-to-date" in output:
                    self.log("=" * 50)
                    self.log("上传成功!", "SUCCESS")
                    self.log("=" * 50)
                    self.update_status("上传成功!")
                    
                    messagebox.showinfo(
                        "上传成功",
                        "代码已成功上传到 GitHub!\n\n下一步:\n1. 点击 '启用 GitHub Actions'\n2. 点击 '触发自动构建'"
                    )
                else:
                    self.log(f"推送失败: {error}", "ERROR")
                    self.update_status("上传失败")
                    messagebox.showerror("上传失败", f"错误: {error}")
                    
            except Exception as e:
                self.log(f"发生错误: {str(e)}", "ERROR")
                self.update_status("发生错误")
            finally:
                self.enable_buttons()
        
        threading.Thread(target=task, daemon=True).start()
    
    def trigger_build(self):
        """触发构建 - 通过提交代码自动触发"""
        if not self.config:
            messagebox.showwarning("未配置", "请先配置项目信息!")
            return
        
        def task():
            self.disable_buttons()
            self.update_status("触发构建中...")
            
            try:
                self.log("=" * 50)
                self.log("触发 GitHub Actions 构建", "INFO")
                self.log("=" * 50)
                
                # 创建小改动触发构建
                timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                
                # 检查README是否存在
                if os.path.exists("README.md"):
                    with open("README.md", "a", encoding="utf-8") as f:
                        f.write(f"\n<!-- Build triggered: {timestamp} -->\n")
                    file_to_commit = "README.md"
                else:
                    # 如果README不存在，创建一个触发文件
                    with open(".build_trigger", "w", encoding="utf-8") as f:
                        f.write(f"Build triggered at {timestamp}\n")
                    file_to_commit = ".build_trigger"
                
                auth_url = self.get_auth_url()
                
                self.run_command(f"git add {file_to_commit}")
                self.run_command(f'git commit -m "chore: trigger build [{timestamp}]"')
                
                if self.config.get("auth_type") == "token":
                    # 使用Token推送
                    self.run_command("git remote set-url origin " + auth_url)
                
                success, output, error = self.run_command("git push origin main")
                
                if success or "Everything up-to-date" in output:
                    self.log("构建已触发!", "SUCCESS")
                    self.log("GitHub Actions 将自动开始构建", "INFO")
                    self.log("预计需要 10-20 分钟", "INFO")
                    self.update_status("构建已触发")
                    
                    # 自动打开Actions页面
                    actions_url = self.get_actions_url()
                    if actions_url:
                        webbrowser.open(actions_url)
                    
                    messagebox.showinfo(
                        "构建已触发",
                        "GitHub Actions 构建已触发!\n\n✅ 自动提交了代码更改\n✅ 构建将在几秒内开始\n✅ 浏览器将打开 Actions 页面\n\n预计需要 10-20 分钟完成"
                    )
                else:
                    self.log(f"触发失败: {error}", "ERROR")
                    self.update_status("触发失败")
                    messagebox.showerror("触发失败", f"错误: {error}\n\n请检查网络连接和认证信息")
                    
            except Exception as e:
                self.log(f"发生错误: {str(e)}", "ERROR")
            finally:
                self.enable_buttons()
        
        threading.Thread(target=task, daemon=True).start()
    
    def get_actions_url(self):
        """获取Actions页面URL"""
        if not self.config:
            return None
        repo_url = self.config.get("repo_url", "")
        if repo_url:
            # 从 https://github.com/user/repo.git 提取 user/repo
            repo_url = repo_url.replace(".git", "")
            return repo_url.replace("github.com", "github.com") + "/actions"
        return None
    
    def get_releases_url(self):
        """获取Releases页面URL"""
        if not self.config:
            return None
        repo_url = self.config.get("repo_url", "")
        if repo_url:
            repo_url = repo_url.replace(".git", "")
            return repo_url + "/releases"
        return None
    
    def view_actions(self):
        """查看构建状态"""
        actions_url = self.get_actions_url()
        if actions_url:
            self.log("打开 GitHub Actions 页面...")
            webbrowser.open(actions_url)
            self.log("已在浏览器中打开", "SUCCESS")
        else:
            messagebox.showwarning("未配置", "请先配置项目信息!")
    
    def create_release(self):
        """创建发布版本"""
        if not self.config:
            messagebox.showwarning("未配置", "请先配置项目信息!")
            return
        
        # 弹出对话框输入版本号
        dialog = tk.Toplevel(self.root)
        dialog.title("创建发布版本")
        dialog.geometry("400x200")
        dialog.transient(self.root)
        dialog.grab_set()
        
        tk.Label(dialog, text="请输入版本号:", font=("Arial", 12)).pack(pady=20)
        
        version_var = tk.StringVar(value="v1.0.0")
        version_entry = tk.Entry(dialog, textvariable=version_var, font=("Arial", 12), width=20)
        version_entry.pack(pady=10)
        
        def do_release():
            version = version_var.get()
            if not version:
                messagebox.showwarning("输入错误", "请输入版本号!")
                return
            
            dialog.destroy()
            
            def task():
                self.disable_buttons()
                self.update_status(f"创建发布 {version}...")
                
                try:
                    self.log("=" * 50)
                    self.log(f"创建发布版本: {version}", "INFO")
                    self.log("=" * 50)
                    
                    auth_url = self.get_auth_url()
                    
                    if self.config.get("auth_type") == "token":
                        self.run_command("git remote set-url origin " + auth_url)
                    
                    # 创建标签
                    self.log(f"创建标签 {version}...")
                    self.run_command(f'git tag -a {version} -m "Release {version}"')
                    self.log("标签创建成功", "SUCCESS")
                    
                    # 推送标签
                    self.log("推送标签到 GitHub...")
                    success, output, error = self.run_command(f"git push origin {version}")
                    
                    if success:
                        self.log("发布创建成功!", "SUCCESS")
                        self.log("GitHub Actions 将自动构建并发布", "INFO")
                        self.update_status(f"发布 {version} 创建成功")
                        
                        releases_url = self.get_releases_url()
                        if releases_url:
                            webbrowser.open(releases_url)
                        
                        messagebox.showinfo(
                            "发布成功",
                            f"版本 {version} 创建成功!\n\nGitHub Actions 将自动:\n✅ 构建所有平台\n✅ 创建 Release 页面\n✅ 上传安装包\n\n⏱️ 预计 15-30 分钟完成"
                        )
                    else:
                        self.log(f"推送失败: {error}", "ERROR")
                        self.update_status("发布失败")
                        messagebox.showerror("发布失败", f"错误: {error}")
                        
                except Exception as e:
                    self.log(f"发生错误: {str(e)}", "ERROR")
                finally:
                    self.enable_buttons()
            
            threading.Thread(target=task, daemon=True).start()
        
        btn_frame = tk.Frame(dialog)
        btn_frame.pack(pady=20)
        
        tk.Button(btn_frame, text="创建", command=do_release, bg="#2ecc71", fg="white", font=("Arial", 11), width=10).pack(side=tk.LEFT, padx=5)
        tk.Button(btn_frame, text="取消", command=dialog.destroy, bg="#95a5a6", fg="white", font=("Arial", 11), width=10).pack(side=tk.LEFT, padx=5)
    
    def open_repo(self):
        """打开GitHub仓库"""
        if self.config:
            repo_url = self.config.get("repo_url", "").replace(".git", "")
            if repo_url:
                self.log("打开 GitHub 仓库...")
                webbrowser.open(repo_url)
                self.log("已在浏览器中打开", "SUCCESS")
                return
        messagebox.showwarning("未配置", "请先配置项目信息!")
    
    def enable_actions(self):
        """启用GitHub Actions"""
        actions_url = self.get_actions_url()
        if actions_url:
            self.log("打开 Actions 启用页面...")
            webbrowser.open(actions_url)
            self.log("请在浏览器中点击 'Enable workflows' 按钮", "WARNING")
            messagebox.showinfo(
                "启用 GitHub Actions",
                "浏览器将打开 Actions 页面\n\n请点击绿色按钮:\n'I understand my workflows, go ahead and enable them'\n\n注意：只需要在第一次使用时启用一次"
            )
        else:
            messagebox.showwarning("未配置", "请先配置项目信息!")

def main():
    """主函数"""
    root = tk.Tk()
    app = GitHubHelper(root)
    
    # 设置图标（如果有的话）
    try:
        root.iconbitmap("icon.ico")
    except:
        pass
    
    # 居中显示
    root.update_idletasks()
    width = root.winfo_width()
    height = root.winfo_height()
    x = (root.winfo_screenwidth() // 2) - (width // 2)
    y = (root.winfo_screenheight() // 2) - (height // 2)
    root.geometry(f'{width}x{height}+{x}+{y}')
    
    root.mainloop()

if __name__ == "__main__":
    main()

