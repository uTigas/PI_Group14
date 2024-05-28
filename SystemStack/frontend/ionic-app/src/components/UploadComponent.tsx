import React, { RefObject, useContext } from 'react';
import { IonButton, IonButtons, IonContent, IonHeader, IonIcon, IonInput, IonItem, IonLabel, IonProgressBar, IonTitle, IonToolbar } from '@ionic/react';
import { attachOutline, cloudUploadOutline } from 'ionicons/icons';
import ApiWrapper from '../support/APIWrapper';

interface UploadComponentProps {
  vaultId: string, 
  user: string | undefined;
  refModal: RefObject<HTMLIonModalElement>;
}

class UploadComponent extends React.Component<UploadComponentProps> {

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

      const formData = new FormData();
      if (vaultId != "") {
        formData.append('vaultId', vaultId) 
      }
      else {
        formData.append('vaultId', "")
      }
      formData.append('file', selectedFile)
      formData.append('fileName', fileName)
      try {
        const response = await ApiWrapper.uploadFile(formData, this.setState);
        if (response)
          console.log('Upload successful:', response.data);
          this.props.refModal.current?.dismiss();
      } catch (error) {
        console.error('Error uploading file:', error);
      } finally {
        this.setState({ uploading: false });
      }
  };

  render() {
    const { uploading, progress, fileName } = this.state;

    return (
      <>
        <IonHeader>
          <IonToolbar mode='ios'>
            <IonButtons slot="start">
              <IonButton onClick={() => this.props.refModal.current?.dismiss()}>Cancel</IonButton>
            </IonButtons>
            <IonTitle>Create Organization</IonTitle>
            <IonButtons slot="end">
              <IonButton strong={true} onClick={this.handleUpload} disabled={uploading}>
                {uploading ? 'Uploading...' : 'Upload'}
              </IonButton>
            </IonButtons>
          </IonToolbar>
        </IonHeader>
        <IonContent className='ion-padding'>
          <IonItem>
          <IonInput label="Filename" labelPlacement="stacked" placeholder='Insert file name' onIonChange={(e) => this.setState({ fileName: e.detail.value })}></IonInput>
          </IonItem>
          <IonItem>
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
                  <IonLabel>{fileName}</IonLabel>
                ) : (
                  <IonLabel>No file selected</IonLabel>
                )}
              </IonItem>
            </label>
          </IonItem>
          {uploading && <IonProgressBar value={progress / 100} />}
      </IonContent>
      </>
    );
  }
}

export default UploadComponent;
