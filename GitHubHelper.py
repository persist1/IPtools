#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
GitHub 自动化助手 - 可视化工具
一键完成 Git 提交、上传、构建、发布
"""

import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox
import subprocess
import threading
import os
import sys
import webbrowser
from datetime import datetime

class GitHubHelper:
    def __init__(self, root):
        self.root = root
        self.root.title("GitHub 自动化助手")
        self.root.geometry("800x600")
        self.root.resizable(True, True)
        
        # 配置
        self.repo_url = "https://github.com/persist1/IPtools.git"
        self.username = "persist1"
        self.email = "317627074@qq.com"
        
        # 创建界面
        self.create_widgets()
        
        # 检查Git
        self.check_git()
    
    def create_widgets(self):
        """创建界面组件"""
        
        # 标题
        title_frame = tk.Frame(self.root, bg="#2c3e50", height=60)
        title_frame.pack(fill=tk.X)
        title_frame.pack_propagate(False)
        
        title_label = tk.Label(
            title_frame,
            text="🚀 GitHub 自动化助手",
            font=("Arial", 20, "bold"),
            bg="#2c3e50",
            fg="white"
        )
        title_label.pack(pady=15)
        
        # 主容器
        main_frame = tk.Frame(self.root, padx=20, pady=20)
        main_frame.pack(fill=tk.BOTH, expand=True)
        
        # 项目信息
        info_frame = tk.LabelFrame(main_frame, text="📦 项目信息", font=("Arial", 12, "bold"), padx=10, pady=10)
        info_frame.pack(fill=tk.X, pady=(0, 10))
        
        tk.Label(info_frame, text=f"仓库: {self.repo_url}", font=("Arial", 10)).pack(anchor=tk.W)
        tk.Label(info_frame, text=f"用户: {self.username}", font=("Arial", 10)).pack(anchor=tk.W)
        
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
    
    def log(self, message, level="INFO"):
        """写入日志"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        
        # 颜色标记
        if level == "SUCCESS":
            prefix = "✓"
            color = "#2ecc71"
        elif level == "ERROR":
            prefix = "✗"
            color = "#e74c3c"
        elif level == "WARNING":
            prefix = "⚠"
            color = "#f39c12"
        else:
            prefix = "ℹ"
            color = "#3498db"
        
        log_entry = f"[{timestamp}] {prefix} {message}\n"
        
        self.log_text.insert(tk.END, log_entry)
        self.log_text.see(tk.END)
        self.root.update()
    
    def update_status(self, text):
        """更新状态栏"""
        self.status_bar.config(text=text)
        self.root.update()
    
    def run_command(self, command, cwd=None):
        """运行命令"""
        try:
            result = subprocess.run(
                command,
                cwd=cwd or os.getcwd(),
                shell=True,
                capture_output=True,
                text=True,
                encoding='utf-8',
                errors='ignore'
            )
            return result.returncode == 0, result.stdout, result.stderr
        except Exception as e:
            return False, "", str(e)
    
    def check_git(self):
        """检查Git是否安装"""
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
                "请先安装 Git!\n\n下载地址: https://git-scm.com/download/win\n\n或运行 install-git.ps1"
            )
    
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
        def task():
            self.disable_buttons()
            self.update_status("正在上传到 GitHub...")
            
            try:
                self.log("=" * 50)
                self.log("开始上传到 GitHub", "INFO")
                self.log("=" * 50)
                
                # 配置Git
                self.log("[1/6] 配置 Git 用户信息...")
                self.run_command(f'git config user.name "{self.username}"')
                self.run_command(f'git config user.email "{self.email}"')
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
                self.run_command(f"git remote add origin {self.repo_url}")
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
                    self.log("没有新的更改或已提交", "WARNING")
                    # 创建空提交
                    self.run_command('git commit --allow-empty -m "Initial commit"')
                
                # 推送
                self.log("[6/6] 推送到 GitHub...")
                self.log("请在弹出窗口输入密码...", "WARNING")
                self.run_command("git branch -M main")
                success, output, error = self.run_command("git push -u origin main")
                
                if success or "Everything up-to-date" in output:
                    self.log("=" * 50)
                    self.log("上传成功!", "SUCCESS")
                    self.log("=" * 50)
                    self.log("下一步: 点击 '启用 GitHub Actions' 按钮", "INFO")
                    self.update_status("上传成功!")
                    
                    messagebox.showinfo(
                        "上传成功",
                        "代码已成功上传到 GitHub!\n\n下一步:\n1. 点击 '启用 GitHub Actions' 按钮\n2. 点击 '触发自动构建' 按钮"
                    )
                else:
                    self.log(f"推送失败: {error}", "ERROR")
                    self.update_status("上传失败")
                    messagebox.showerror("上传失败", f"错误: {error}\n\n请检查用户名和密码")
                    
            except Exception as e:
                self.log(f"发生错误: {str(e)}", "ERROR")
                self.update_status("发生错误")
            finally:
                self.enable_buttons()
        
        threading.Thread(target=task, daemon=True).start()
    
    def trigger_build(self):
        """触发构建"""
        def task():
            self.disable_buttons()
            self.update_status("触发构建中...")
            
            try:
                self.log("=" * 50)
                self.log("触发 GitHub Actions 构建", "INFO")
                self.log("=" * 50)
                
                # 创建小改动触发构建
                timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                with open("README.md", "a", encoding="utf-8") as f:
                    f.write(f"\n<!-- Build triggered: {timestamp} -->\n")
                
                self.run_command("git add README.md")
                self.run_command(f'git commit -m "chore: trigger build [{timestamp}]"')
                success, output, error = self.run_command("git push origin main")
                
                if success or "Everything up-to-date" in output:
                    self.log("构建已触发!", "SUCCESS")
                    self.log("访问 Actions 页面查看构建进度", "INFO")
                    self.update_status("构建已触发")
                    
                    # 自动打开Actions页面
                    webbrowser.open(f"https://github.com/persist1/IPtools/actions")
                    
                    messagebox.showinfo(
                        "构建已触发",
                        "GitHub Actions 构建已触发!\n\n浏览器将自动打开 Actions 页面\n构建需要 10-20 分钟"
                    )
                else:
                    self.log(f"触发失败: {error}", "ERROR")
                    self.update_status("触发失败")
                    
            except Exception as e:
                self.log(f"发生错误: {str(e)}", "ERROR")
            finally:
                self.enable_buttons()
        
        threading.Thread(target=task, daemon=True).start()
    
    def view_actions(self):
        """查看构建状态"""
        self.log("打开 GitHub Actions 页面...")
        webbrowser.open("https://github.com/persist1/IPtools/actions")
        self.log("已在浏览器中打开", "SUCCESS")
    
    def create_release(self):
        """创建发布版本"""
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
                    
                    # 创建标签
                    self.log(f"创建标签 {version}...")
                    success1, _, error1 = self.run_command(f'git tag -a {version} -m "Release {version}"')
                    
                    if success1:
                        self.log("标签创建成功", "SUCCESS")
                    else:
                        self.log(f"标签创建警告: {error1}", "WARNING")
                    
                    # 推送标签
                    self.log("推送标签到 GitHub...")
                    success2, output, error2 = self.run_command(f"git push origin {version}")
                    
                    if success2:
                        self.log("发布创建成功!", "SUCCESS")
                        self.log("GitHub Actions 将自动构建并发布", "INFO")
                        self.update_status(f"发布 {version} 创建成功")
                        
                        webbrowser.open("https://github.com/persist1/IPtools/releases")
                        
                        messagebox.showinfo(
                            "发布成功",
                            f"版本 {version} 创建成功!\n\nGitHub Actions 将自动:\n- 构建所有平台\n- 创建 Release 页面\n- 上传安装包\n\n预计 15-30 分钟完成"
                        )
                    else:
                        self.log(f"推送失败: {error2}", "ERROR")
                        self.update_status("发布失败")
                        
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
        self.log("打开 GitHub 仓库...")
        webbrowser.open("https://github.com/persist1/IPtools")
        self.log("已在浏览器中打开", "SUCCESS")
    
    def enable_actions(self):
        """启用GitHub Actions"""
        self.log("打开 Actions 启用页面...")
        webbrowser.open("https://github.com/persist1/IPtools/actions")
        self.log("请在浏览器中点击 'Enable workflows' 按钮", "WARNING")
        messagebox.showinfo(
            "启用 GitHub Actions",
            "浏览器将打开 Actions 页面\n\n请点击绿色按钮:\n'I understand my workflows, go ahead and enable them'"
        )

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

