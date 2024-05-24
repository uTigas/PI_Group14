import React, { useContext, useEffect, useState } from 'react';
import { IonContent, IonInput, IonButton, IonItem, IonLabel, IonTextarea, IonSelect, IonSelectOption, IonIcon, IonText } from '@ionic/react';
import ApiWrapper from '../support/APIWrapper';
import { add, mail, mailOutline, warning } from 'ionicons/icons';

interface AddMemberContainerProps {
  organization : string,
  fetchOrganization: () => void;
}

const AddMemberContainer: React.FC<AddMemberContainerProps> = ({ organization, fetchOrganization }) => {
  const [username, setUsername] = useState('');
  const [formValid, setFormValid] = useState(false);
  const [inviteStatus, setInviteStatus] = useState("");
  const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    const formData = new URLSearchParams();
    formData.append('username', username);
    formData.append('organization', organization);
    const response = await ApiWrapper.inviteMember(formData)
    if (response instanceof Error){
      setInviteStatus("Invalid Username.") 
    }
    else{
      setInviteStatus("Invite sent successfully!")
    }
    fetchOrganization()
  };

  const validateForm = () => {
    setFormValid(username.trim().length > 0);
  };
  
  useEffect(() => {
    validateForm(); 
  }, [username]);


  return (
      <form onSubmit={handleSubmit} className='ion-text-center'> 
          <IonItem>
            <IonLabel position="floating">Username</IonLabel>
            <IonInput
              type="text"
              value={username}
              onIonChange={(e) => {
                setUsername(e.detail.value!);
              }}
            />
          </IonItem>
          
          <IonButton expand="block" type="submit" disabled={!formValid}>Invite <IonIcon icon={mailOutline} className='ion-padding-start'></IonIcon></IonButton>
          <IonItem>
            <div className='ion-text-end'>
              <IonText color={"warning"}>{inviteStatus}</IonText>
            </div>
          </IonItem>
      </form>
  );
};

export default AddMemberContainer;
