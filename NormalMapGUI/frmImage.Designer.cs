namespace NormalMap
{
    partial class frmImage
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
            this.pbxTexture = new System.Windows.Forms.PictureBox();
            this.btnSave = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.pbxTexture)).BeginInit();
            this.SuspendLayout();
            // 
            // pbxTexture
            // 
            this.pbxTexture.Location = new System.Drawing.Point(12, 50);
            this.pbxTexture.Name = "pbxTexture";
            this.pbxTexture.Size = new System.Drawing.Size(584, 503);
            this.pbxTexture.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pbxTexture.TabIndex = 0;
            this.pbxTexture.TabStop = false;
            // 
            // btnSave
            // 
            this.btnSave.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnSave.Location = new System.Drawing.Point(278, 12);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(75, 32);
            this.btnSave.TabIndex = 1;
            this.btnSave.Text = "Save";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // frmImage
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(608, 565);
            this.Controls.Add(this.btnSave);
            this.Controls.Add(this.pbxTexture);
            this.Name = "frmImage";
            this.Text = "Image";
            this.Load += new System.EventHandler(this.frmImage_Load);
            this.Resize += new System.EventHandler(this.frmImage_Resize);
            ((System.ComponentModel.ISupportInitialize)(this.pbxTexture)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pbxTexture;
        private System.Windows.Forms.Button btnSave;
    }
}