namespace NormalMap
{
    partial class frmNormalMapGenerator
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnLoadFile = new System.Windows.Forms.Button();
            this.pbxTexture = new System.Windows.Forms.PictureBox();
            this.gpbOptions = new System.Windows.Forms.GroupBox();
            this.nudScale = new System.Windows.Forms.NumericUpDown();
            this.lblScale = new System.Windows.Forms.Label();
            this.chkWrapBorder = new System.Windows.Forms.CheckBox();
            this.chkInvertY = new System.Windows.Forms.CheckBox();
            this.chkInvertX = new System.Windows.Forms.CheckBox();
            this.nudStrength = new System.Windows.Forms.NumericUpDown();
            this.lblStrength = new System.Windows.Forms.Label();
            this.lblFilter = new System.Windows.Forms.Label();
            this.ddlFilter = new System.Windows.Forms.ComboBox();
            this.chkDepthMapOnAlpha = new System.Windows.Forms.CheckBox();
            this.gpbTexture = new System.Windows.Forms.GroupBox();
            this.gpbNormalMap = new System.Windows.Forms.GroupBox();
            this.btnSaveNormalMap = new System.Windows.Forms.Button();
            this.pbxNormalMap = new System.Windows.Forms.PictureBox();
            this.btnClear = new System.Windows.Forms.Button();
            this.gpbDepthMap = new System.Windows.Forms.GroupBox();
            this.btnSaveDepthMap = new System.Windows.Forms.Button();
            this.pbxDepthMap = new System.Windows.Forms.PictureBox();
            this.lblAbout = new System.Windows.Forms.Label();
            this.gpbABout = new System.Windows.Forms.GroupBox();
            this.gpbFile = new System.Windows.Forms.GroupBox();
            ((System.ComponentModel.ISupportInitialize)(this.pbxTexture)).BeginInit();
            this.gpbOptions.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudScale)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudStrength)).BeginInit();
            this.gpbTexture.SuspendLayout();
            this.gpbNormalMap.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pbxNormalMap)).BeginInit();
            this.gpbDepthMap.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pbxDepthMap)).BeginInit();
            this.gpbABout.SuspendLayout();
            this.gpbFile.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnLoadFile
            // 
            this.btnLoadFile.Location = new System.Drawing.Point(6, 15);
            this.btnLoadFile.Name = "btnLoadFile";
            this.btnLoadFile.Size = new System.Drawing.Size(150, 29);
            this.btnLoadFile.TabIndex = 0;
            this.btnLoadFile.Text = "Load File";
            this.btnLoadFile.UseVisualStyleBackColor = true;
            this.btnLoadFile.Click += new System.EventHandler(this.btnLoadFile_Click);
            // 
            // pbxTexture
            // 
            this.pbxTexture.Location = new System.Drawing.Point(6, 21);
            this.pbxTexture.Name = "pbxTexture";
            this.pbxTexture.Size = new System.Drawing.Size(411, 301);
            this.pbxTexture.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pbxTexture.TabIndex = 1;
            this.pbxTexture.TabStop = false;
            this.pbxTexture.Click += new System.EventHandler(this.pbxTexture_Click);
            // 
            // gpbOptions
            // 
            this.gpbOptions.Controls.Add(this.nudScale);
            this.gpbOptions.Controls.Add(this.lblScale);
            this.gpbOptions.Controls.Add(this.chkWrapBorder);
            this.gpbOptions.Controls.Add(this.chkInvertY);
            this.gpbOptions.Controls.Add(this.chkInvertX);
            this.gpbOptions.Controls.Add(this.nudStrength);
            this.gpbOptions.Controls.Add(this.lblStrength);
            this.gpbOptions.Controls.Add(this.lblFilter);
            this.gpbOptions.Controls.Add(this.ddlFilter);
            this.gpbOptions.Location = new System.Drawing.Point(457, 60);
            this.gpbOptions.Name = "gpbOptions";
            this.gpbOptions.Size = new System.Drawing.Size(425, 328);
            this.gpbOptions.TabIndex = 3;
            this.gpbOptions.TabStop = false;
            this.gpbOptions.Text = "Options";
            // 
            // nudScale
            // 
            this.nudScale.DecimalPlaces = 2;
            this.nudScale.Location = new System.Drawing.Point(299, 26);
            this.nudScale.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            65536});
            this.nudScale.Name = "nudScale";
            this.nudScale.Size = new System.Drawing.Size(120, 22);
            this.nudScale.TabIndex = 8;
            this.nudScale.Value = new decimal(new int[] {
            10,
            0,
            0,
            65536});
            this.nudScale.ValueChanged += new System.EventHandler(this.nudScale_ValueChanged);
            // 
            // lblScale
            // 
            this.lblScale.AutoSize = true;
            this.lblScale.Location = new System.Drawing.Point(246, 28);
            this.lblScale.Name = "lblScale";
            this.lblScale.Size = new System.Drawing.Size(47, 17);
            this.lblScale.TabIndex = 7;
            this.lblScale.Text = "Scale:";
            // 
            // chkWrapBorder
            // 
            this.chkWrapBorder.AutoSize = true;
            this.chkWrapBorder.Location = new System.Drawing.Point(9, 190);
            this.chkWrapBorder.Name = "chkWrapBorder";
            this.chkWrapBorder.Size = new System.Drawing.Size(151, 21);
            this.chkWrapBorder.TabIndex = 6;
            this.chkWrapBorder.Text = "Wrap Border Pixels";
            this.chkWrapBorder.UseVisualStyleBackColor = true;
            this.chkWrapBorder.CheckedChanged += new System.EventHandler(this.chkWrapBorder_CheckedChanged);
            // 
            // chkInvertY
            // 
            this.chkInvertY.AutoSize = true;
            this.chkInvertY.Location = new System.Drawing.Point(9, 154);
            this.chkInvertY.Name = "chkInvertY";
            this.chkInvertY.Size = new System.Drawing.Size(78, 21);
            this.chkInvertY.TabIndex = 5;
            this.chkInvertY.Text = "Invert Y";
            this.chkInvertY.UseVisualStyleBackColor = true;
            this.chkInvertY.CheckedChanged += new System.EventHandler(this.chkInvertY_CheckedChanged);
            // 
            // chkInvertX
            // 
            this.chkInvertX.AutoSize = true;
            this.chkInvertX.Location = new System.Drawing.Point(9, 118);
            this.chkInvertX.Name = "chkInvertX";
            this.chkInvertX.Size = new System.Drawing.Size(78, 21);
            this.chkInvertX.TabIndex = 4;
            this.chkInvertX.Text = "Invert X";
            this.chkInvertX.UseVisualStyleBackColor = true;
            this.chkInvertX.CheckedChanged += new System.EventHandler(this.chkInvertX_CheckedChanged);
            // 
            // nudStrength
            // 
            this.nudStrength.DecimalPlaces = 2;
            this.nudStrength.Increment = new decimal(new int[] {
            5,
            0,
            0,
            65536});
            this.nudStrength.Location = new System.Drawing.Point(96, 74);
            this.nudStrength.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
            this.nudStrength.Name = "nudStrength";
            this.nudStrength.Size = new System.Drawing.Size(120, 22);
            this.nudStrength.TabIndex = 3;
            this.nudStrength.Value = new decimal(new int[] {
            10,
            0,
            0,
            65536});
            this.nudStrength.ValueChanged += new System.EventHandler(this.nudStrength_ValueChanged);
            // 
            // lblStrength
            // 
            this.lblStrength.AutoSize = true;
            this.lblStrength.Location = new System.Drawing.Point(6, 76);
            this.lblStrength.Name = "lblStrength";
            this.lblStrength.Size = new System.Drawing.Size(66, 17);
            this.lblStrength.TabIndex = 2;
            this.lblStrength.Text = "Strength:";
            // 
            // lblFilter
            // 
            this.lblFilter.AutoSize = true;
            this.lblFilter.Location = new System.Drawing.Point(6, 28);
            this.lblFilter.Name = "lblFilter";
            this.lblFilter.Size = new System.Drawing.Size(43, 17);
            this.lblFilter.TabIndex = 1;
            this.lblFilter.Text = "Filter:";
            // 
            // ddlFilter
            // 
            this.ddlFilter.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ddlFilter.FormattingEnabled = true;
            this.ddlFilter.Location = new System.Drawing.Point(55, 25);
            this.ddlFilter.Name = "ddlFilter";
            this.ddlFilter.Size = new System.Drawing.Size(161, 24);
            this.ddlFilter.TabIndex = 0;
            this.ddlFilter.SelectedIndexChanged += new System.EventHandler(this.ddlFilter_SelectedIndexChanged);
            // 
            // chkDepthMapOnAlpha
            // 
            this.chkDepthMapOnAlpha.AutoSize = true;
            this.chkDepthMapOnAlpha.Location = new System.Drawing.Point(184, 333);
            this.chkDepthMapOnAlpha.Name = "chkDepthMapOnAlpha";
            this.chkDepthMapOnAlpha.Size = new System.Drawing.Size(233, 21);
            this.chkDepthMapOnAlpha.TabIndex = 7;
            this.chkDepthMapOnAlpha.Text = "Depth Map on Alpha component";
            this.chkDepthMapOnAlpha.UseVisualStyleBackColor = true;
            this.chkDepthMapOnAlpha.CheckedChanged += new System.EventHandler(this.chkDepthMapOnAlpha_CheckedChanged);
            // 
            // gpbTexture
            // 
            this.gpbTexture.Controls.Add(this.pbxTexture);
            this.gpbTexture.Location = new System.Drawing.Point(13, 60);
            this.gpbTexture.Name = "gpbTexture";
            this.gpbTexture.Size = new System.Drawing.Size(425, 328);
            this.gpbTexture.TabIndex = 4;
            this.gpbTexture.TabStop = false;
            this.gpbTexture.Text = "Diffuse / Height Map";
            // 
            // gpbNormalMap
            // 
            this.gpbNormalMap.Controls.Add(this.chkDepthMapOnAlpha);
            this.gpbNormalMap.Controls.Add(this.btnSaveNormalMap);
            this.gpbNormalMap.Controls.Add(this.pbxNormalMap);
            this.gpbNormalMap.Location = new System.Drawing.Point(13, 400);
            this.gpbNormalMap.Name = "gpbNormalMap";
            this.gpbNormalMap.Size = new System.Drawing.Size(425, 368);
            this.gpbNormalMap.TabIndex = 5;
            this.gpbNormalMap.TabStop = false;
            this.gpbNormalMap.Text = "Normal Map";
            // 
            // btnSaveNormalMap
            // 
            this.btnSaveNormalMap.Location = new System.Drawing.Point(6, 328);
            this.btnSaveNormalMap.Name = "btnSaveNormalMap";
            this.btnSaveNormalMap.Size = new System.Drawing.Size(150, 29);
            this.btnSaveNormalMap.TabIndex = 8;
            this.btnSaveNormalMap.Text = "Save";
            this.btnSaveNormalMap.UseVisualStyleBackColor = true;
            this.btnSaveNormalMap.Click += new System.EventHandler(this.btnSaveNormalMap_Click);
            // 
            // pbxNormalMap
            // 
            this.pbxNormalMap.Location = new System.Drawing.Point(6, 21);
            this.pbxNormalMap.Name = "pbxNormalMap";
            this.pbxNormalMap.Size = new System.Drawing.Size(411, 301);
            this.pbxNormalMap.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pbxNormalMap.TabIndex = 1;
            this.pbxNormalMap.TabStop = false;
            this.pbxNormalMap.Click += new System.EventHandler(this.pbxNormalMap_Click);
            // 
            // btnClear
            // 
            this.btnClear.Location = new System.Drawing.Point(267, 15);
            this.btnClear.Name = "btnClear";
            this.btnClear.Size = new System.Drawing.Size(150, 29);
            this.btnClear.TabIndex = 6;
            this.btnClear.Text = "Clear";
            this.btnClear.UseVisualStyleBackColor = true;
            this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
            // 
            // gpbDepthMap
            // 
            this.gpbDepthMap.Controls.Add(this.btnSaveDepthMap);
            this.gpbDepthMap.Controls.Add(this.pbxDepthMap);
            this.gpbDepthMap.Location = new System.Drawing.Point(457, 400);
            this.gpbDepthMap.Name = "gpbDepthMap";
            this.gpbDepthMap.Size = new System.Drawing.Size(425, 368);
            this.gpbDepthMap.TabIndex = 7;
            this.gpbDepthMap.TabStop = false;
            this.gpbDepthMap.Text = "Depth Map";
            // 
            // btnSaveDepthMap
            // 
            this.btnSaveDepthMap.Location = new System.Drawing.Point(6, 328);
            this.btnSaveDepthMap.Name = "btnSaveDepthMap";
            this.btnSaveDepthMap.Size = new System.Drawing.Size(150, 29);
            this.btnSaveDepthMap.TabIndex = 9;
            this.btnSaveDepthMap.Text = "Save";
            this.btnSaveDepthMap.UseVisualStyleBackColor = true;
            this.btnSaveDepthMap.Click += new System.EventHandler(this.btnSaveDepthMap_Click);
            // 
            // pbxDepthMap
            // 
            this.pbxDepthMap.Location = new System.Drawing.Point(6, 21);
            this.pbxDepthMap.Name = "pbxDepthMap";
            this.pbxDepthMap.Size = new System.Drawing.Size(411, 301);
            this.pbxDepthMap.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pbxDepthMap.TabIndex = 1;
            this.pbxDepthMap.TabStop = false;
            this.pbxDepthMap.Click += new System.EventHandler(this.pbxDepthMap_Click);
            // 
            // lblAbout
            // 
            this.lblAbout.AutoSize = true;
            this.lblAbout.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblAbout.Location = new System.Drawing.Point(6, 21);
            this.lblAbout.Name = "lblAbout";
            this.lblAbout.Size = new System.Drawing.Size(367, 17);
            this.lblAbout.TabIndex = 8;
            this.lblAbout.Text = "Created by Bruno Vergilio (brunodeovergilio@gmail.com)";
            // 
            // gpbABout
            // 
            this.gpbABout.Controls.Add(this.lblAbout);
            this.gpbABout.Location = new System.Drawing.Point(457, 4);
            this.gpbABout.Name = "gpbABout";
            this.gpbABout.Size = new System.Drawing.Size(423, 50);
            this.gpbABout.TabIndex = 9;
            this.gpbABout.TabStop = false;
            this.gpbABout.Text = "About";
            // 
            // gpbFile
            // 
            this.gpbFile.Controls.Add(this.btnLoadFile);
            this.gpbFile.Controls.Add(this.btnClear);
            this.gpbFile.Location = new System.Drawing.Point(13, 4);
            this.gpbFile.Name = "gpbFile";
            this.gpbFile.Size = new System.Drawing.Size(425, 50);
            this.gpbFile.TabIndex = 10;
            this.gpbFile.TabStop = false;
            this.gpbFile.Text = "File";
            // 
            // frmNormalMapGenerator
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(891, 780);
            this.Controls.Add(this.gpbFile);
            this.Controls.Add(this.gpbABout);
            this.Controls.Add(this.gpbDepthMap);
            this.Controls.Add(this.gpbNormalMap);
            this.Controls.Add(this.gpbTexture);
            this.Controls.Add(this.gpbOptions);
            this.MaximizeBox = false;
            this.Name = "frmNormalMapGenerator";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Normal Map Generator";
            this.Load += new System.EventHandler(this.frmNormalMapGenerator_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pbxTexture)).EndInit();
            this.gpbOptions.ResumeLayout(false);
            this.gpbOptions.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudScale)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudStrength)).EndInit();
            this.gpbTexture.ResumeLayout(false);
            this.gpbNormalMap.ResumeLayout(false);
            this.gpbNormalMap.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pbxNormalMap)).EndInit();
            this.gpbDepthMap.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pbxDepthMap)).EndInit();
            this.gpbABout.ResumeLayout(false);
            this.gpbABout.PerformLayout();
            this.gpbFile.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnLoadFile;
        private System.Windows.Forms.PictureBox pbxTexture;
        private System.Windows.Forms.GroupBox gpbOptions;
        private System.Windows.Forms.GroupBox gpbTexture;
        private System.Windows.Forms.GroupBox gpbNormalMap;
        private System.Windows.Forms.PictureBox pbxNormalMap;
        private System.Windows.Forms.Button btnClear;
        private System.Windows.Forms.GroupBox gpbDepthMap;
        private System.Windows.Forms.PictureBox pbxDepthMap;
        private System.Windows.Forms.ComboBox ddlFilter;
        private System.Windows.Forms.NumericUpDown nudStrength;
        private System.Windows.Forms.Label lblStrength;
        private System.Windows.Forms.Label lblFilter;
        private System.Windows.Forms.CheckBox chkInvertY;
        private System.Windows.Forms.CheckBox chkInvertX;
        private System.Windows.Forms.CheckBox chkWrapBorder;
        private System.Windows.Forms.CheckBox chkDepthMapOnAlpha;
        private System.Windows.Forms.Button btnSaveNormalMap;
        private System.Windows.Forms.Button btnSaveDepthMap;
        private System.Windows.Forms.NumericUpDown nudScale;
        private System.Windows.Forms.Label lblScale;
        private System.Windows.Forms.Label lblAbout;
        private System.Windows.Forms.GroupBox gpbABout;
        private System.Windows.Forms.GroupBox gpbFile;
    }
}

