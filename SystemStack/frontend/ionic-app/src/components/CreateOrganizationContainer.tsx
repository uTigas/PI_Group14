import React, { useContext, useState } from 'react';
import { IonContent, IonInput, IonButton, IonItem, IonLabel, IonTextarea, IonSelect, IonSelectOption } from '@ionic/react';
import axios from 'axios';
import ApiWrapper from '../support/APIWrapper';

const CreateOrganizationContainer: React.FC = () => {
  const [name, setName] = useState('');
  const [description, setDescription] = useState('');
  const [size, setSize] = useState('');
  const [formValid, setFormValid] = useState(false);

  const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    const formData = new URLSearchParams();
    formData.append('name', name);
    formData.append('description', description);
    formData.append('size', size);
    ApiWrapper.createOrganization(formData);
    console.log('Submitted:', { name, description, size });

  };

  const validateForm = () => {
    setFormValid(name.trim().length > 0  && size.trim().length > 0);
  };

  return (
      <form onSubmit={handleSubmit}> 
          <IonItem>
            <IonLabel position="floating">Name</IonLabel>
            <IonInput
              type="text"
              value={name}
              onIonChange={(e) => {
                setName(e.detail.value!);
                validateForm();
              }}
            />
          </IonItem>

          <IonItem>
            <IonLabel position="floating">Description</IonLabel>
            <IonTextarea
              value={description}
              onIonChange={(e) => {
                setDescription(e.detail.value!);
                validateForm();
              }}
            />
          </IonItem>

          <IonItem>
            <IonLabel position="stacked">Size</IonLabel> 
            <IonSelect
                label='Select Size'
                value={size}
                onIonChange={(e) => {
                setSize(e.detail.value!);
                validateForm();
                }}
                placeholder="Select Size"
            >
                <IonSelectOption value="small">Small</IonSelectOption>
                <IonSelectOption value="medium">Medium</IonSelectOption>
                <IonSelectOption value="large">Large</IonSelectOption>
            </IonSelect>
            </IonItem>

          <IonButton expand="block" type="submit" disabled={!formValid}>Create</IonButton>
      </form>
  );
};

export default CreateOrganizationContainer;
