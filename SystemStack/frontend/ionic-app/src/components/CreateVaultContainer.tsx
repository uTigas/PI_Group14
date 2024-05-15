import React, { useContext, useEffect, useState } from 'react';
import { IonContent, IonInput, IonButton, IonItem, IonLabel, IonTextarea, IonSelect, IonSelectOption, IonCheckbox, IonPopover, IonIcon, IonTitle } from '@ionic/react';
import ApiWrapper from './APIWrapper';
import Common from './Common';
import { help, helpCircle } from 'ionicons/icons';

interface ChildComponentProps {
  organizationId: string,
  fetchOrganization: () => void;

}

const CreateVaultContainer: React.FC<ChildComponentProps> = ({ organizationId, fetchOrganization }) => {

  const [name, setName] = useState('');
  const [description, setDescription] = useState('');
  const [formValid, setFormValid] = useState(false);
  const [view, setView] = useState(Common.DEFAULT_ROLES.EVERYONE);
  const [edit, setEdit] = useState(Common.DEFAULT_ROLES.ADMIN);
  const [manage, setManage] = useState(Common.DEFAULT_ROLES.ADMIN);

  const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    const formData = new URLSearchParams();
    formData.append('name', name);
    formData.append('description', description);
    formData.append('view', view);
    formData.append('edit', edit);
    formData.append('manage', manage);
    formData.append('organization', organizationId)
    console.log({name, description, view, edit, manage, organizationId});
    ApiWrapper.createVault(formData).then(response => fetchOrganization());
  };

  useEffect(() => {
    validateForm(); 
  }, [name, description]);

  const validateForm = () => {
    setFormValid(name.trim().length > 0 && description.trim().length > 0);
  };

  return (
      <form onSubmit={handleSubmit}> 
          <IonItem className='ion-padding-top'>
            <IonLabel color='warning'>Definition</IonLabel>
          </IonItem>
          <IonItem>
            <IonLabel position="floating">Name:</IonLabel>
            <IonInput
              type="text"
              value={name}
              onIonChange={(e) => {
                setName(e.detail.value!);
              }}
            />
          </IonItem>

          <IonItem>
            <IonLabel position="floating">Description:</IonLabel>
            <IonTextarea
              value={description}
              onIonChange={(e) => {
                setDescription(e.detail.value!);
              }}
            />
          </IonItem>
          <IonItem className='ion-padding-top'>
            <IonLabel color='tertiary'>Permissions<IonIcon icon={helpCircle} size='medium'></IonIcon></IonLabel>
          </IonItem>
          <IonItem>
            <IonSelect label='View' interface='popover' onIonChange={(e) => {setView(e.detail.value)}} value={Common.DEFAULT_ROLES.EVERYONE}>
              <IonSelectOption disabled={true}>
                <IonLabel style={{ color: '#808080', fontStyle: 'italic' }}>--Default Roles--</IonLabel>
              </IonSelectOption>
              <IonSelectOption value={Common.DEFAULT_ROLES.EVERYONE}>Everyone</IonSelectOption>
              <IonSelectOption value={Common.DEFAULT_ROLES.ADMIN}>Admins</IonSelectOption>
              <IonSelectOption value={Common.DEFAULT_ROLES.ME}>Just Me</IonSelectOption>
              <IonSelectOption disabled={true}>
                <IonLabel style={{ color: '#808080', fontStyle: 'italic' }}>--Personalized Roles--</IonLabel>
              </IonSelectOption>
            </IonSelect>
          </IonItem>

          <IonItem>
            <IonSelect label='Edit' interface='popover' onIonChange={(e) => {setEdit(e.detail.value)}} value={Common.DEFAULT_ROLES.ADMIN}>
              <IonSelectOption disabled={true}>
                <IonLabel style={{ color: '#808080', fontStyle: 'italic' }}>--Default Roles--</IonLabel>
              </IonSelectOption>
              <IonSelectOption value={Common.DEFAULT_ROLES.EVERYONE}>Everyone</IonSelectOption>
              <IonSelectOption value={Common.DEFAULT_ROLES.ADMIN}>Admins</IonSelectOption>
              <IonSelectOption value={Common.DEFAULT_ROLES.ME}>Just Me</IonSelectOption>
              <IonSelectOption disabled={true}>
                <IonLabel style={{ color: '#808080', fontStyle: 'italic' }}>--Personalized Roles--</IonLabel>
              </IonSelectOption>
            </IonSelect>
          </IonItem>
          <IonItem>
            <IonSelect label='Manage' interface='popover' onIonChange={(e) => {setManage(e.detail.value)}} value={Common.DEFAULT_ROLES.ADMIN}>
              <IonSelectOption disabled={true}>
                <IonLabel style={{ color: '#808080', fontStyle: 'italic' }}>--Default Roles--</IonLabel>
              </IonSelectOption>
              <IonSelectOption value={Common.DEFAULT_ROLES.EVERYONE}>Everyone</IonSelectOption>
              <IonSelectOption value={Common.DEFAULT_ROLES.ADMIN}>Admins</IonSelectOption>
              <IonSelectOption value={Common.DEFAULT_ROLES.ME}>Just Me</IonSelectOption>
              <IonSelectOption disabled={true}>
                <IonLabel style={{ color: '#808080', fontStyle: 'italic' }}>--Personalized Roles--</IonLabel>
              </IonSelectOption>
            </IonSelect>
          </IonItem>
          <IonButton expand="block" type="submit" disabled={!formValid}>Create</IonButton>
      </form>
  );
};

export default CreateVaultContainer;
