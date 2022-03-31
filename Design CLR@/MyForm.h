#pragma once

namespace Project4 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Сводка для MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}

               private:
                System::Windows::Forms::Button ^ button_send;

               protected: 

               private:


               protected:
               private:


               private:
                System::Windows::Forms::ListBox ^ user_list;

               private:
                System::Windows::Forms::RichTextBox ^ message_read;

               private:


               private:


               private:


               private:


               private:


               private:


               private:


               private:


               private:


               private:


               private:


               private:


               private:


               private:


               private:


               private:
                System::Windows::Forms::RichTextBox ^ message_send;

               private:
                System::Windows::Forms::Label ^ label_message_send;

               private:
                System::Windows::Forms::Label ^ label_message_read;

               private:


               private:


               private:


               private:
                System::Windows::Forms::Label ^ label_user_list;

               private:


               private:
                System::Windows::Forms::Label ^ label_name;

               private:


               private:
                System::Windows::Forms::Button ^ button_server;

               private:
                System::Windows::Forms::Button ^ button_client;

               private:
                System::Windows::Forms::Button ^ button_log;

               private:


               private:


               private:


               private:


               private:
                System::Windows::Forms::Label ^ label_ip;

               private:
                System::Windows::Forms::Label ^ label_port;

               private:
                System::Windows::Forms::TextBox ^ textbox_name;

               private:
                System::Windows::Forms::TextBox ^ textbox_ip;

               private:
                System::Windows::Forms::TextBox ^ textbox_port;

               private:
                System::Windows::Forms::Button ^ button_user_count;

               private:


               private:


               private:


               private:


               private:


               private:


               private:


               private:


               private:


	private:
		/// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
                  this->button_send = (gcnew System::Windows::Forms::Button());
                  this->user_list = (gcnew System::Windows::Forms::ListBox());
                  this->message_read =
                      (gcnew System::Windows::Forms::RichTextBox());
                  this->message_send =
                      (gcnew System::Windows::Forms::RichTextBox());
                  this->label_message_send =
                      (gcnew System::Windows::Forms::Label());
                  this->label_message_read =
                      (gcnew System::Windows::Forms::Label());
                  this->label_user_list =
                      (gcnew System::Windows::Forms::Label());
                  this->label_name = (gcnew System::Windows::Forms::Label());
                  this->button_server =
                      (gcnew System::Windows::Forms::Button());
                  this->button_client =
                      (gcnew System::Windows::Forms::Button());
                  this->button_log = (gcnew System::Windows::Forms::Button());
                  this->label_ip = (gcnew System::Windows::Forms::Label());
                  this->label_port = (gcnew System::Windows::Forms::Label());
                  this->textbox_name =
                      (gcnew System::Windows::Forms::TextBox());
                  this->textbox_ip = (gcnew System::Windows::Forms::TextBox());
                  this->textbox_port =
                      (gcnew System::Windows::Forms::TextBox());
                  this->button_user_count =
                      (gcnew System::Windows::Forms::Button());
                  this->SuspendLayout();
                  //
                  // button_send
                  //
                  this->button_send->Location =
                      System::Drawing::Point(305, 104);
                  this->button_send->Name = L"button_send";
                  this->button_send->Size = System::Drawing::Size(70, 23);
                  this->button_send->TabIndex = 1;
                  this->button_send->Text = L"Отправить";
                  this->button_send->UseVisualStyleBackColor = true;
                  //
                  // user_list
                  //
                  this->user_list->FormattingEnabled = true;
                  this->user_list->Location = System::Drawing::Point(381, 130);
                  this->user_list->Name = L"user_list";
                  this->user_list->Size = System::Drawing::Size(151, 264);
                  this->user_list->TabIndex = 7;
                  //
                  // message_read
                  //
                  this->message_read->Location =
                      System::Drawing::Point(12, 193);
                  this->message_read->Name = L"message_read";
                  this->message_read->Size = System::Drawing::Size(363, 201);
                  this->message_read->TabIndex = 5;
                  this->message_read->Text = L"";
                  //
                  // message_send
                  //
                  this->message_send->Location =
                      System::Drawing::Point(12, 130);
                  this->message_send->Name = L"message_send";
                  this->message_send->Size = System::Drawing::Size(363, 44);
                  this->message_send->TabIndex = 4;
                  this->message_send->Text = L"";
                  //
                  // label_message_send
                  //
                  this->label_message_send->AutoSize = true;
                  this->label_message_send->Location =
                      System::Drawing::Point(12, 114);
                  this->label_message_send->Name = L"label_message_send";
                  this->label_message_send->Size =
                      System::Drawing::Size(143, 13);
                  this->label_message_send->TabIndex = 13;
                  this->label_message_send->Text = L"Окно отправки сообщений";
                  //
                  // label_message_read
                  //
                  this->label_message_read->AutoSize = true;
                  this->label_message_read->Location =
                      System::Drawing::Point(9, 177);
                  this->label_message_read->Name = L"label_message_read";
                  this->label_message_read->Size =
                      System::Drawing::Size(58, 13);
                  this->label_message_read->TabIndex = 14;
                  this->label_message_read->Text = L"Окно чата";
                  //
                  // label_user_list
                  //
                  this->label_user_list->AutoSize = true;
                  this->label_user_list->Location =
                      System::Drawing::Point(394, 114);
                  this->label_user_list->Name = L"label_user_list";
                  this->label_user_list->Size = System::Drawing::Size(124, 13);
                  this->label_user_list->TabIndex = 15;
                  this->label_user_list->Text = L"Список пользователей";
                  //
                  // label_name
                  //
                  this->label_name->AutoSize = true;
                  this->label_name->Location = System::Drawing::Point(105, 24);
                  this->label_name->Name = L"label_name";
                  this->label_name->Size = System::Drawing::Size(29, 13);
                  this->label_name->TabIndex = 16;
                  this->label_name->Text = L"Имя";
                  //
                  // button_server
                  //
                  this->button_server->Location =
                      System::Drawing::Point(15, 21);
                  this->button_server->Name = L"button_server";
                  this->button_server->Size = System::Drawing::Size(75, 23);
                  this->button_server->TabIndex = 17;
                  this->button_server->Text = L"Сервер";
                  this->button_server->UseVisualStyleBackColor = true;
                  //
                  // button_client
                  //
                  this->button_client->Location =
                      System::Drawing::Point(15, 65);
                  this->button_client->Name = L"button_client";
                  this->button_client->Size = System::Drawing::Size(75, 23);
                  this->button_client->TabIndex = 18;
                  this->button_client->Text = L"Клиент";
                  this->button_client->UseVisualStyleBackColor = true;
                  this->button_client->Click += gcnew System::EventHandler(
                      this, &MyForm::button_client_Click);
                  //
                  // button_log
                  //
                  this->button_log->Location = System::Drawing::Point(457, 24);
                  this->button_log->Name = L"button_log";
                  this->button_log->Size = System::Drawing::Size(75, 23);
                  this->button_log->TabIndex = 19;
                  this->button_log->Text = L"Лог";
                  this->button_log->UseVisualStyleBackColor = true;
                  //
                  // label_ip
                  //
                  this->label_ip->AutoSize = true;
                  this->label_ip->Location = System::Drawing::Point(105, 45);
                  this->label_ip->Name = L"label_ip";
                  this->label_ip->Size = System::Drawing::Size(50, 13);
                  this->label_ip->TabIndex = 20;
                  this->label_ip->Text = L"IP-адрес";
                  //
                  // label_port
                  //
                  this->label_port->AutoSize = true;
                  this->label_port->Location = System::Drawing::Point(105, 68);
                  this->label_port->Name = L"label_port";
                  this->label_port->Size = System::Drawing::Size(32, 13);
                  this->label_port->TabIndex = 21;
                  this->label_port->Text = L"Порт";
                  //
                  // textbox_name
                  //
                  this->textbox_name->Location =
                      System::Drawing::Point(164, 21);
                  this->textbox_name->Name = L"textbox_name";
                  this->textbox_name->Size = System::Drawing::Size(100, 20);
                  this->textbox_name->TabIndex = 22;
                  //
                  // textbox_ip
                  //
                  this->textbox_ip->Font = (gcnew System::Drawing::Font(
                      L"Calibri", 9.75F, System::Drawing::FontStyle::Regular,
                      System::Drawing::GraphicsUnit::Point,
                      static_cast<System::Byte>(204)));
                  this->textbox_ip->Location = System::Drawing::Point(164, 42);
                  this->textbox_ip->Name = L"textbox_ip";
                  this->textbox_ip->Size = System::Drawing::Size(100, 23);
                  this->textbox_ip->TabIndex = 23;
                  this->textbox_ip->Text = L"127.0.0.1";
                  //
                  // textbox_port
                  //
                  this->textbox_port->Font = (gcnew System::Drawing::Font(
                      L"Calibri", 9.75F, System::Drawing::FontStyle::Regular,
                      System::Drawing::GraphicsUnit::Point,
                      static_cast<System::Byte>(204)));
                  this->textbox_port->Location =
                      System::Drawing::Point(164, 65);
                  this->textbox_port->Name = L"textbox_port";
                  this->textbox_port->Size = System::Drawing::Size(100, 23);
                  this->textbox_port->TabIndex = 24;
                  this->textbox_port->Text = L"600";
                  //
                  // button_user_count
                  //
                  this->button_user_count->Location =
                      System::Drawing::Point(381, 88);
                  this->button_user_count->Name = L"button_user_count";
                  this->button_user_count->Size =
                      System::Drawing::Size(151, 23);
                  this->button_user_count->TabIndex = 25;
                  this->button_user_count->Text = L"Кол-во пользователей";
                  this->button_user_count->UseVisualStyleBackColor = true;
                  //
                  // MyForm
                  //
                  this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
                  this->AutoScaleMode =
                      System::Windows::Forms::AutoScaleMode::Font;
                  this->ClientSize = System::Drawing::Size(544, 401);
                  this->Controls->Add(this->button_user_count);
                  this->Controls->Add(this->textbox_port);
                  this->Controls->Add(this->textbox_ip);
                  this->Controls->Add(this->textbox_name);
                  this->Controls->Add(this->label_port);
                  this->Controls->Add(this->label_ip);
                  this->Controls->Add(this->button_log);
                  this->Controls->Add(this->button_client);
                  this->Controls->Add(this->button_server);
                  this->Controls->Add(this->label_name);
                  this->Controls->Add(this->label_user_list);
                  this->Controls->Add(this->label_message_read);
                  this->Controls->Add(this->label_message_send);
                  this->Controls->Add(this->user_list);
                  this->Controls->Add(this->message_read);
                  this->Controls->Add(this->message_send);
                  this->Controls->Add(this->button_send);
                  this->MaximizeBox = false;
                  this->MinimizeBox = false;
                  this->Name = L"MyForm";
                  this->Text = L"Чат по локальной сети";
                  this->ResumeLayout(false);
                  this->PerformLayout();
                }
#pragma endregion
               private:
                System::Void label1_Click(System::Object ^ sender,
                                          System::EventArgs ^ e) {}

               private:
                System::Void настройкиToolStripMenuItem_Click(
                    System::Object ^ sender, System::EventArgs ^ e) {}

               private:
                System::Void label3_Click(System::Object ^ sender,
                                          System::EventArgs ^ e) {}

               private:
                System::Void button3_Click(System::Object ^ sender,
                                           System::EventArgs ^ e) {}

               private:
                System::Void textBox2_TextChanged(System::Object ^ sender,
                                                  System::EventArgs ^ e) {}

               private:
                System::Void button_client_Click(System::Object ^ sender,
                                                 System::EventArgs ^ e) {}
         };
         }
