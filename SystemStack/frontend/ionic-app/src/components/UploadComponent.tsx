import React, { useContext } from 'react';
import { IonButton, IonIcon, IonInput, IonItem, IonProgressBar } from '@ionic/react';
import { cloudUploadOutline } from 'ionicons/icons';
import ApiWrapper from '../support/APIWrapper';
import { User } from '../App';

interface UploadComponentProps {
  vaultId: string, 
  user: string | undefined;
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
    });

    if (this.state.fileName === "")
        this.setState({
            fileName: event.target.files[0].name,
        })
  };

  handleUpload = async () => {
    const { selectedFile, fileName } = this.state;
    const { vaultId, user } = this.props
    if (!selectedFile) return;

    this.setState({ uploading: true });

    if (user){
      const formData = new FormData();
      formData.append('vaultId', String(vaultId))
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
    const { uploading, progress } = this.state;

    return (
        <div>
          <IonItem>
            <IonInput className="ion-margin-bottom" placeholder='Insert file name' onIonChange={(e) => this.setState({fileName: e.detail.value})}></IonInput>
          </IonItem>
          <IonItem>
            <input type="file" onChange={this.handleFileChange} />
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

export default UploadComponent;
