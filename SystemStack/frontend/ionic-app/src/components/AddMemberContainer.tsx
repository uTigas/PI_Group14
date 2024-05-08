import React, { useContext, useState } from 'react';
import { IonContent, IonInput, IonButton, IonItem, IonLabel, IonTextarea, IonSelect, IonSelectOption, IonIcon } from '@ionic/react';
import axios from 'axios';
import ApiWrapper from './APIWrapper';
import { add } from 'ionicons/icons';

const AddMemberContainer: React.FC = () => {
  const [username, setUsername] = useState('');
  const [formValid, setFormValid] = useState(false);

  const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    const formData = new URLSearchParams();
    formData.append('username', username);

    await axios.post(ApiWrapper.backendURI + "vault/create", formData, {
      withCredentials: true, 
      headers: {
      'Content-Type': 'application/x-www-form-urlencoded'
      }
    })
    .then(response => console.log(response))
    .catch(error => console.log(error))

    console.log('Submitted:', { username });

  };

  const validateForm = () => {
    setFormValid(username.trim().length > 0);
  };

  return (
      <form onSubmit={handleSubmit} className='ion-text-center'> 
          <IonItem>
            <IonLabel position="floating">Username</IonLabel>
            <IonInput
              type="text"
              value={username}
              onIonChange={(e) => {
                setUsername(e.detail.value!);
                validateForm();
              }}
            />
          </IonItem>
          
          <IonButton  type="submit" disabled={!formValid}><IonIcon icon={add} size='medium'></IonIcon></IonButton>
          
      </form>
  );
};

export default AddMemberContainer;
