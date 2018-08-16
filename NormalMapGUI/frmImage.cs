using System;
using System.Windows.Forms;
using FreeImageAPI;
using System.Drawing;

namespace NormalMap
{
    public partial class frmImage : Form
    {
        public Bitmap bm { get; set; }
        public NormalMap normalMap { get; set; }

        public enum ImageType
        {
            Texture,
            NormalMap,
            DepthMap
        }

        public ImageType imageType { get; set; }

        public frmImage(String type, Bitmap bm, ImageType imageType, NormalMap normalMap)
        {
            this.Text = type;
            this.bm = bm;
            this.normalMap = normalMap;
            this.imageType = imageType;

            InitializeComponent();
        }

        ~frmImage()
        {
            bm = null;
            normalMap = null;
        }

        private void frmImage_Load(object sender, EventArgs e)
        {
            pbxTexture.Image = bm;
        }

        private void frmImage_Resize(object sender, EventArgs e)
        {
            btnSave.Location = new Point(this.Size.Width / 2 - btnSave.Size.Width / 2, btnSave.Location.Y);
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "Image Files|*.bmp;*.jpg;*.png;*.gif;*.tga; | All files|*.*";
            if (sfd.ShowDialog() == DialogResult.OK)
            {
                bool result = true;
                switch(imageType)
                {
                    case ImageType.Texture:
                        result = normalMap.SaveTexture(sfd.FileName);
                        break;
                    case ImageType.NormalMap:
                        result = normalMap.SaveNormalMap(sfd.FileName);
                        break;
                    case ImageType.DepthMap:
                        result = normalMap.SaveDepthMap(sfd.FileName);
                        break;
                }
                if (!result)
                {
                    MessageBox.Show("Unable to save file! Check directory permissions or file format chosen!");
                }
            }
        }
    }
}
