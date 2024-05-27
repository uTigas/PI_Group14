import React, { useContext } from 'react';
import { IonButton, IonIcon, IonInput, IonItem, IonLabel, IonProgressBar } from '@ionic/react';
import { attachOutline, cloudUploadOutline } from 'ionicons/icons';
import ApiWrapper from '../support/APIWrapper';

interface UploadComponentProps {
  vaultId: string, 
  user: string | undefined;
}

class UploadComponentOrg extends React.Component<UploadComponentProps> {

  constructor(props: UploadComponentProps) {
    super(props);
  }
  state = {
    selectedFile: null,
    fileName : "",
    uploading: false,
    progress: 0,
  };

  handleFileChange = (event: any) => {
    this.setState({
      selectedFile: event.target.files[0],
      fileName: event.target.files[0].name,
    });
  };

  handleUpload = async () => {
    const { selectedFile, fileName } = this.state;
    const { vaultId, user } = this.props
    if (!selectedFile) return;

    this.setState({ uploading: true });

    if (user){
      const formData = new FormData();
      formData.append('vaultId', "")
      formData.append('file', selectedFile)
      formData.append('fileName', fileName)
      try {
        const response = await ApiWrapper.uploadFile(formData, this.setState);
        if (response)
          console.log('Upload successful:', response.data);
      } catch (error) {
        console.error('Error uploading file:', error);
      } finally {
        this.setState({ uploading: false });
      }
    }
  };

  render() {
    const { uploading, progress, fileName } = this.state;

    return (
        <div>
          <IonItem>
            <IonInput placeholder='Insert file name' onIonChange={(e) => this.setState({fileName: e.detail.value})}></IonInput>
          </IonItem>
          <IonItem lines="none">
            <IonLabel position="stacked" className='ion-padding-vertical'>Select File</IonLabel>
            <input 
              type="file" 
              onChange={this.handleFileChange} 
              style={{ display: 'none' }} 
              accept="image/*, .pdf, .doc, .docx" // Add any file types you want to accept
              id="fileInput" 
            />
            <label htmlFor="fileInput" style={{ width: '100%' }}>
              <IonItem lines="none">
                <IonIcon icon={attachOutline} slot="start" />
                {fileName ? (
                  <p>{fileName}</p>
                ) : (
                  <p>No file selected</p>
                )}
              </IonItem>
            </label>
          </IonItem>
          <IonItem>
            <IonButton onClick={this.handleUpload} disabled={uploading}>
              {uploading ? 'Uploading...' : 'Upload'}
              <IonIcon icon={cloudUploadOutline} className='ion-margin-start'></IonIcon>
            </IonButton>
          </IonItem>
          {uploading && <IonProgressBar value={progress / 100} />}
      </div>
    );
  }
}

export default UploadComponentOrg;
