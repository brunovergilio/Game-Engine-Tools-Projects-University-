using System;
using System.Drawing;
using System.Windows.Forms;
using FreeImageAPI;
using System.Collections.Generic;

namespace NormalMap
{
    public partial class frmNormalMapGenerator : Form
    {
        // Bitmaps for the Texture, Normal Map and Depth Map
        private Bitmap bmTexture;
        private Bitmap bmNormalMap;
        private Bitmap bmDepthMap;
        private Bitmap bmSpecularMap;

        // NormalMap class
        private NormalMap normalMap;

        public frmNormalMapGenerator()
        {
            InitializeComponent();

            bmTexture = null;
            bmNormalMap = null;
            bmDepthMap = null;
            bmSpecularMap = null;

            normalMap = new NormalMap();

            // Disable everything since there's nothing loaded
            ToggleOptions(false);
        }

        ~frmNormalMapGenerator()
        {
            ClearTextures();

            bmTexture = null;
            bmNormalMap = null;
            bmDepthMap = null;
            bmSpecularMap = null;
        }

        // Dispose of everything
        private void ClearTextures()
        {
            if (bmTexture != null)
            {
                bmTexture.Dispose();
                bmTexture = null;
            }

            if (bmNormalMap != null)
            {
                bmNormalMap.Dispose();
                bmNormalMap = null;
            }

            if (bmDepthMap != null)
            {
                bmDepthMap.Dispose();
                bmDepthMap = null;
            }

            if (bmSpecularMap != null)
            {
                bmSpecularMap.Dispose();
                bmSpecularMap = null;
            }
        }

        // Will issue a call to NormalMap.Generate, passing the bitmaps to be have data written to them
        private void GenerateNormalMap()
        {
            if (pbxTexture.Image != null)
            {
                normalMap.Generate(out bmNormalMap, out bmDepthMap);
                pbxNormalMap.Image = bmNormalMap;
                pbxDepthMap.Image = bmDepthMap;
            }
        }

        // Enable / Disable all the option controls
        private void ToggleOptions(bool enabled)
        {
            ddlFilter.Enabled = enabled;
            nudStrength.Enabled = enabled;
            chkInvertX.Enabled = enabled;
            chkInvertY.Enabled = enabled;
            chkWrapBorder.Enabled = enabled;
            chkDepthMapOnAlpha.Enabled = enabled;
            nudScale.Enabled = enabled;
            btnSaveNormalMap.Enabled = enabled;
            btnSaveDepthMap.Enabled = enabled;
        }

        private void btnLoadFile_Click(object sender, EventArgs e)
        {
            // Loads up a file dialog
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Image Files|*.bmp;*.jpg;*.png;*.gif;*.tga; | All files|*.*";
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                // If there were previous textures, dispose of them
                ClearTextures();

                // Load a temporary file
                FIBITMAP tmpFile = FreeImage.LoadEx(ofd.FileName);
                if (tmpFile.IsNull)
                {
                    MessageBox.Show("Couldn't load the selected image!");
                    return;
                }
                // Convert it to 32 bits
                FIBITMAP imgFile = FreeImage.ConvertTo32Bits(tmpFile);

                // Dispose of the temp file
                FreeImage.UnloadEx(ref tmpFile);

                // Get a .NET bitmap from FreeImage
                bmTexture = FreeImage.GetBitmap(imgFile);

                // Get the bytes as well as other information from the file
                normalMap.SetTexture(imgFile);

                // Dispose of another FreeImage file
                FreeImage.UnloadEx(ref imgFile);

                // Put it on screen
                pbxTexture.Image = bmTexture;

                // Generate the normal map with the default settings
                GenerateNormalMap();

                // Enable the options
                ToggleOptions(true);
            }
        }

        // Get rid of everything
        private void btnClear_Click(object sender, EventArgs e)
        {
            pbxTexture.Image = null;
            pbxNormalMap.Image = null;
            pbxDepthMap.Image = null;

            ClearTextures();

            ToggleOptions(false);

            ddlFilter.SelectedIndex = 0;
            normalMap.SetDefaultFilterValues(NormalMap.EdgeFilter.Sobel3X3);
            normalMap.Filter = NormalMap.EdgeFilter.Sobel3X3;

            chkInvertX.Checked = false; normalMap.InvertX = false;
            chkInvertY.Checked = false; normalMap.InvertY = false;
            chkWrapBorder.Checked = false; normalMap.WrapBorder = false;
            chkDepthMapOnAlpha.Checked = false; normalMap.DepthMapOnAlpha = false;

            nudScale.Value = (decimal)normalMap.GetDefaultScale(NormalMap.EdgeFilter.Sobel3X3);
            nudStrength.Value = (decimal)1.0f; normalMap.Strength = 1.0f;
        }

        // Open the texture in their full size on another form
        private void pbxTexture_Click(object sender, EventArgs e)
        {
            if (pbxTexture.Image != null)
            {
                frmImage textureForm = new frmImage(gpbTexture.Text, bmTexture, frmImage.ImageType.Texture, normalMap);
                textureForm.Show();
            }
        }

        // Open the normal map in their full size on another form
        private void pbxNormalMap_Click(object sender, EventArgs e)
        {
            if (pbxNormalMap.Image != null)
            {
                frmImage textureForm = new frmImage(gpbNormalMap.Text, bmNormalMap, frmImage.ImageType.NormalMap, normalMap);
                textureForm.Show();
            }
        }

        // Open the depth map in their full size on another form
        private void pbxDepthMap_Click(object sender, EventArgs e)
        {
            if (pbxDepthMap.Image != null)
            {
                frmImage textureForm = new frmImage(gpbDepthMap.Text, bmDepthMap, frmImage.ImageType.DepthMap, normalMap);
                textureForm.Show();
            }
        }

        private void frmNormalMapGenerator_Load(object sender, EventArgs e)
        {
            List<KeyValuePair<String, int>> filters = new List<KeyValuePair<string, int>>();
            filters.Add(new KeyValuePair<string, int>("Sobel 3x3", 0));
            filters.Add(new KeyValuePair<string, int>("Sobel 5x5", 1));
            filters.Add(new KeyValuePair<string, int>("Sobel 7x7", 2));
            filters.Add(new KeyValuePair<string, int>("Sobel 9x9", 3));

            ddlFilter.DataSource = filters;
            ddlFilter.DisplayMember = "Key";
            ddlFilter.ValueMember = "Value";
            ddlFilter.SelectedIndex = 0;
            ddlFilter.SelectedValue = 0;
        }

        private void ddlFilter_SelectedIndexChanged(object sender, EventArgs e)
        {
            KeyValuePair<string, int> option = (KeyValuePair<string, int>)ddlFilter.SelectedItem;
            normalMap.Filter = (NormalMap.EdgeFilter)option.Value;

            normalMap.SetDefaultFilterValues(normalMap.Filter);
            nudScale.Value = (decimal)normalMap.GetDefaultScale(normalMap.Filter);

            GenerateNormalMap();
        }

        private void nudStrength_ValueChanged(object sender, EventArgs e)
        {
            normalMap.Strength = (float)nudStrength.Value;

            GenerateNormalMap();
        }

        private void chkInvertX_CheckedChanged(object sender, EventArgs e)
        {
            normalMap.InvertX = chkInvertX.Checked;

            GenerateNormalMap();
        }

        private void chkInvertY_CheckedChanged(object sender, EventArgs e)
        {
            normalMap.InvertY = chkInvertY.Checked;

            GenerateNormalMap();
        }

        private void chkWrapBorder_CheckedChanged(object sender, EventArgs e)
        {
            normalMap.WrapBorder = chkWrapBorder.Checked;

            GenerateNormalMap();
        }

        private void btnSaveNormalMap_Click(object sender, EventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "Image Files|*.bmp;*.jpg;*.png;*.gif;*.tga; | All files|*.*";
            if (sfd.ShowDialog() == DialogResult.OK)
            {
                if (!normalMap.SaveNormalMap(sfd.FileName))
                {
                    MessageBox.Show("Unable to save file! Check directory permissions or file format chosen!");
                }
            }
        }

        private void btnSaveDepthMap_Click(object sender, EventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "Image Files|*.bmp;*.jpg;*.png;*.gif;*.tga; | All files|*.*";
            if (sfd.ShowDialog() == DialogResult.OK)
            {
                if (!normalMap.SaveDepthMap(sfd.FileName))
                {
                    MessageBox.Show("Unable to save file! Check directory permissions or file format chosen!");
                }
            }
        }

        private void chkDepthMapOnAlpha_CheckedChanged(object sender, EventArgs e)
        {
            normalMap.DepthMapOnAlpha = chkDepthMapOnAlpha.Checked;
        }

        private void nudScale_ValueChanged(object sender, EventArgs e)
        {
            KeyValuePair<string, int> option = (KeyValuePair<string, int>)ddlFilter.SelectedItem;

            normalMap.UpdateFilter((NormalMap.EdgeFilter)option.Value, (float)nudScale.Value);

            GenerateNormalMap();
        }
    }
}
